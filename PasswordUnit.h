#ifndef PASSWORDUNIT_H
#define PASSWORDUNIT_H

#include <iostream>
#include <string>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cstring>

using namespace CryptoPP;
using namespace std;

class PasswordUnit {
public:
    std::string domain_name;
    std::string login;
    std::string password;

    
    PasswordUnit() : key(AES::DEFAULT_KEYLENGTH) {
        std::memset(iv, 0, AES::BLOCKSIZE);
    }

    
    PasswordUnit(const std::string& domain, const std::string& login, const std::string& password, const SecByteBlock& key, const CryptoPP::byte* iv)
        : domain_name(domain), login(login), key(key) {
        std::memcpy(this->iv, iv, AES::BLOCKSIZE);
        this->password = encryptPassword(password);
    }
       PasswordUnit(const std::string& domain, const std::string& login, const std::string& password)
        : domain_name(domain), login(login),  
          key(CryptoPP::SecByteBlock(16)), iv() {
            this->password = encryptPassword(password);
    }

    std::string getPassword() const {
        return decryptPassword(password);
    }

private:
    SecByteBlock key;
    CryptoPP::byte iv[AES::BLOCKSIZE];

    std::string encryptPassword(const std::string& plaintext) const {
        std::string ciphertext;
        try {
            CBC_Mode<AES>::Encryption encryption;
            encryption.SetKeyWithIV(key, key.size(), iv);

            StringSource(plaintext, true, new StreamTransformationFilter(encryption, new StringSink(ciphertext)));
        } catch (const Exception& e) {
            cerr << "Encryption error: " << e.what() << endl;
            throw;
        }

        return ciphertext;
    }

    std::string decryptPassword(const std::string& ciphertext) const {
        std::string decryptedtext;
        try {
            CBC_Mode<AES>::Decryption decryption;
            decryption.SetKeyWithIV(key, key.size(), iv);

            StringSource(ciphertext, true, new StreamTransformationFilter(decryption, new StringSink(decryptedtext)));
        } catch (const Exception& e) {
            cerr << "Decryption error: " << e.what() << endl;
            throw;
        }

        return decryptedtext;
    }
};

#endif // PASSWORDUNIT_H
