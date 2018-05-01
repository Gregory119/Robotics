#include "license.h"

#include "utils.h"

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h> //autoseededrandompool
#include <cryptopp/sha3.h>

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>

// user harware code:
// hardware message => hash => encrypt with key1 (encrypted hw digest/hash)

// license key:
// encrypted hw hash => decrypt with key1 => encrypt with key2 (license key)

constexpr int hash_byte_size = 32;
static const std::string hw_user_key_ob = "opa@%n7(5v+$02=!";
static const std::string hw_license_key_ob = "~@_&^!&s{*dg%a]s";

constexpr char s_license_file[] = "license";
constexpr char s_license_request_file[] = "license_request";

//----------------------------------------------------------------------//
CryptoPP::byte* CastToByte(const char* ptr)
{
  return reinterpret_cast<CryptoPP::byte*>(const_cast<char*>(ptr));
}

//----------------------------------------------------------------------//
bool Stream::start()
{
  // 1) generate a unique harware number using the cpu info and eth mac.
  std::string cpu_info = Utils::CpuInfo();
  if (cpu_info.empty())
    {
      assert(false);
      return false;
    }

  std::ifstream mac_file("/sys/class/net/eth0/address");
  if (mac_file.fail())
    {
      assert(false);
      return false;
    }

  std::string mac;
  std::getline(mac_file, mac, mac_file.widen(EOF));
  if (mac.empty())
    {
      assert(false);
      return false;
    }
  
  // obfuscate the cpu info and mac to get a unique hardware message
  std::string hw = cpu_info;
  hw += mac;
  
  // hash the hardware message to generate a unique hardware id
  std::unique_ptr<CryptoPP::SHA3> sha3(new CryptoPP::SHA3_256);
  std::string hw_id(hash_byte_size, 0);
  sha3->
    CalculateDigest(CastToByte(hw_id.data()),
		    CastToByte(hw.data()),
		    hw.size());
  std::cout << "The hardware id is: " << hw_id << std::endl;

  // 2) encrypt the hashed hardware message to provide to the user, in order for them to apply for a license
  // Using AES in GCM mode for encryption and authentication
  // get hw_user_key
  std::string hw_user_key = ob(hw_user_key_ob);
  std::cout << "The hw user key is: " << hw_user_key << std::endl;
  assert(hw_user_key.size() == CryptoPP::AES::DEFAULT_KEYLENGTH);
  
  // Generate user hardware initialization vector
  CryptoPP::AutoSeededRandomPool prng;
  std::string hw_user_iv(CryptoPP::AES::BLOCKSIZE, 0); // initialization vector is commonly the block size
  prng.GenerateBlock(CastToByte(hw_user_iv.data()),
		     hw_user_iv.size());

  // authentication encryption of hardware id
  std::string cipher_text;
  using AES_TYPE = CryptoPP::GCM<CryptoPP::AES>;
  std::unique_ptr<AES_TYPE::Encryption> enc(new AES_TYPE::Encryption);
  enc->SetKeyWithIV(CastToByte(hw_user_key.data()),
		    hw_user_key.size(),
		    CastToByte(hw_user_iv.data()),
		    hw_user_iv.size());
  
  CryptoPP::AuthenticatedEncryptionFilter aef(*enc,
					      new CryptoPP::StringSink(cipher_text));
  aef.Put(CastToByte(hw_id.data()), hw_id.size());
  aef.MessageEnd(); // The cipher text is now a concatenation of the encrypted data and the authenticator

  std::cout << "cipher text: " << cipher_text << std::endl;
  
  // Prepend the cipher text with the generated IV
  /*cipher_text.insert(cipher_text.begin(),
		     hw_user_iv.begin(),
		     hw_user_iv.end());*/
  
  // hash-based message authentication (using for data integrity) of the cipher text

  std::string user_req;
  user_req += hw_user_iv;
  user_req += cipher_text;

  std::cout << "user text: " << user_req << "\n"
	    << "user text size: " << user_req.size()
	    << std::endl;

  // Write the user license request code to a file for the user to upload when requesting for a license.
  std::ofstream license_req_file(s_license_request_file,
				 std::ios::out | std::ios::binary);
  if (license_req_file.fail())
    {
      assert(false);
      return false;
    }
  license_req_file << user_req;
  if (license_req_file.fail())
    {
      assert(false);
      return false;
    }
  license_req_file.flush();
  
  // 3) extract the hardware id from the user license key, by decrypting it, and compare it with the generated hardware id for verification
  // get user license key
  std::string license_key = getU();
  if (license_key.empty())
    {
      assert(false);
      return false;
    }
  
  // attack the cipher text or initialization vector => must result in a verification failure
  //cipher_text.at(0) |= 0x0F;
  //cipher_text.at(cipher_text.size()-1) |= 0x0F;
  //hw_user_iv.at(hw_user_iv.size()-1) |= 0x0F;
  
  // decrypt user license key
  std::string dec_message;
  try
    {
      std::unique_ptr<AES_TYPE::Decryption> dec(new AES_TYPE::Decryption);
      dec->SetKeyWithIV(reinterpret_cast<CryptoPP::byte*>(const_cast<char*>(hw_user_key.data())),
		       hw_user_key.size(),
		       reinterpret_cast<CryptoPP::byte*>(const_cast<char*>(hw_user_iv.data())),
		       hw_user_iv.size());

      CryptoPP::AuthenticatedDecryptionFilter adf(*dec,
						  new CryptoPP::StringSink(dec_message));

      adf.Put(CastToByte(cipher_text.data()), cipher_text.size());
      adf.MessageEnd();
    }
  catch (...)
    {
      // authenticity/verification failure
      std::cout << "authenticity/verification failure" << std::endl;
      return false;
    }
  
  if (hw_id == dec_message)
    {
      return true;
    }
  
  // decrypt the encrypted hardware message
  // compare the derived hardware message from the user with the generated one
  return false;
}

//----------------------------------------------------------------------//
std::string Stream::getU()
{
  try
    {
      std::ifstream file(s_license_file,
			 std::ios::in | std::ios::binary);
      if (file.fail())
	{
	  return std::string();
	}

      std::string user_key;
      file >> user_key;
      return user_key;
    }
  catch (...)
    {
      assert(false);
      return std::string();
    }
}

//----------------------------------------------------------------------//
std::string Stream::ob(const std::string& key)
{
  std::string out;
  out.reserve(key.size());
  
  for (size_t i=0; i<key.size(); i+=2)
    {
      out.push_back(key.at(i));
    }

  for (size_t i=1; i<key.size(); i+=2)
    {
      out.push_back(key.at(i));
    }

  assert(out.size() == key.size());
  return out;
}

//----------------------------------------------------------------------//
std::string Stream::getParam(const std::string& filename,
			     const std::string& match)
{  
  std::ifstream file(filename);
  if (file.fail())
    {
      return std::string();
    }
  
  std::string line;
  size_t line_match_pos = std::string::npos;
  std::iostream::pos_type match_previous_ipos = 0;

  try
    {
      while (1)
	{
	  match_previous_ipos = file.tellg();

	  std::getline(file, line);
	  line_match_pos = line.find(match);
	  if (line_match_pos != std::string::npos) // not found yet
	    {
	      break;
	    }

	  if (file.fail())
	    {
	      return std::string();
	    }
	}
    }
  catch (...)
    {
      return std::string();
    }
  file.clear(); // clear the failed state due to reaching the eof
  size_t line_value_pos = line_match_pos+match.length();
  std::iostream::pos_type val_pos = match_previous_ipos+static_cast<std::streamoff>(line_value_pos);

  // extract the serial number
  file.seekg(val_pos); // set input indicator
  std::string val;
  file >> val; // leading white space is skipped by default
  return val;
}
