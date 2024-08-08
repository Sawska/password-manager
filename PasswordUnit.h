#ifndef PASSWORDUNIT_H
#define PASSWORDUNIT_H

#include <iostream>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

using namespace CryptoPP;
using namespace std;

using CryptoPPByte = CryptoPP::byte; // Create an alias

class PasswordUnit {
public:
    std::string domain_name;
    std::string login;
    std::string password;
     PasswordUnit() : domain_name(""), login(""), password("") {}
    PasswordUnit(const std::string& domain, const std::string& login, const std::string& password)
        : domain_name(domain), login(login) {
        this->password = encryptPassword(password);
    }

    std::string getPassword() const {
        return decryptPassword(password);
    }

private:
    std::string key = "0123456789abcdef"; // 16-byte key for AES-128
    std::string iv = "abcdef0123456789";  // 16-byte IV for AES-128

    std::string encryptPassword(const std::string& plainText) const {
        std::string cipherText;
        try {
            CFB_Mode<AES>::Encryption encryption((CryptoPPByte*)key.data(), key.size(), (CryptoPPByte*)iv.data());
            StringSource(plainText, true,
                new StreamTransformationFilter(encryption,
                    new StringSink(cipherText)
                ) // StreamTransformationFilter
            ); // StringSource
        } catch (const Exception& e) {
            cerr << "Encryption error: " << e.what() << endl;
        }
        return cipherText;
    }

    std::string decryptPassword(const std::string& cipherText) const {
        std::string recoveredText;
        try {
            CFB_Mode<AES>::Decryption decryption((CryptoPPByte*)key.data(), key.size(), (CryptoPPByte*)iv.data());
            StringSource(cipherText, true,
                new StreamTransformationFilter(decryption,
                    new StringSink(recoveredText)
                ) // StreamTransformationFilter
            ); // StringSource
        } catch (const Exception& e) {
            cerr << "Decryption error: " << e.what() << endl;
        }
        return recoveredText;
    }
};

#endif // PASSWORDUNIT_H
