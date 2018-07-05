/*================================================================
 *     Copyright (c) 2015年 lanhu. All rights reserved.
 *
 *   文件名称：EncDec.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2015年01月27日
 *   描    述：AES
 *
 #pragma once
 ================================================================*/
#ifndef __ENCDEC_H__
#define __ENCDEC_H__

#include <iostream>
#include <openssl/aes.h>
#include <openssl/md5.h>
#include "ostype.h"

using namespace std;
class CAes
{
public:
    CAes() {}
    CAes(const string& strKey);

    static CAes* getInstance();
    void init(const string& strKey);
    
    int Encrypt(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen);
    int Decrypt(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen);
    void Free(char* pData);
    //int EncryptCBC128(const char* pInData, uint32_t nInLen, string& strOutData, uint32_t& nOutLen);
    //int DecryptCBC128(const char* pInData, uint32_t nInLen, string& strOutData, uint32_t& nOutLen);
    int EncryptCBC128(const string& strInData, string& strOutData);
    int DecryptCBC128(const string& strInData, string& strOutData);
    
private:
    AES_KEY m_cEncKey;
    AES_KEY m_cDecKey;

    static CAes * _ptrAes;
    
};

class CMd5
{
public:
    static void MD5_Calculate (const char* pContent, unsigned int nLen,char* md5);
};

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
#endif /*defined(__ENCDEC_H__) */
