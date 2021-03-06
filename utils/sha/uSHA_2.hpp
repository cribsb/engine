/*!
 * \file uSHA_2.hpp
 * \brief \b Classes: \a uSHA_2
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "defines.hpp"

#include <array>
#include <stdint.h>
#include <string>
#include <vector>

namespace e_engine {

enum HASH_FUNCTION { SHA2_224, SHA2_256, SHA2_384, SHA2_512 };

/*!
 * \brief class for calculating SHA-2 (SHA-224, SHA-256, SHA-384, SHA-512) hashes
 */
class UTILS_API uSHA_2 {
 public:
 private:
   HASH_FUNCTION vType;

   unsigned long int vBlockCounter_ulI; //!< Number of calculated blocks
   unsigned int      vBlockSize_uI;     //!< Block size in bytes

   bool vEnded_B; //!< Is the hash complete?

   std::string vResult_str; //!< The final hash

   uint32_t h_512[8];
   uint64_t h_1024[8];

   std::array<unsigned char, 64>  vBuffer512_A_uC;
   std::array<unsigned char, 128> vBuffer1024_A_uC;

   std::array<unsigned char, 64>::iterator  vCurrentPos512_A_IT;
   std::array<unsigned char, 128>::iterator vCurrentPos1024_A_IT;

   void init();

   void padd512();
   void padd1024();

   bool test( HASH_FUNCTION _type, std::string const &_message, std::string const &_result );

   uSHA_2() {}

 public:
   uSHA_2( HASH_FUNCTION _type );

   bool add( std::string const &_message );
   bool add( std::vector<unsigned char> const &_binary );

   void block( std::array<unsigned char, 64> const &_data );
   void block( std::array<unsigned char, 128> const &_data );

   std::vector<unsigned char> end();
   std::string get( bool _space = false );

   std::vector<unsigned char> quickHash( HASH_FUNCTION _type, std::string _message );
   std::vector<unsigned char> quickHash( HASH_FUNCTION _type, std::vector<unsigned char> _binary );

   std::vector<unsigned char> operator()( HASH_FUNCTION _type, std::string _message ) {
      return quickHash( _type, _message );
   }
   std::vector<unsigned char> operator()( HASH_FUNCTION              _type,
                                          std::vector<unsigned char> _binary ) {
      return quickHash( _type, _binary );
   }

   unsigned int getHashLength();

   bool selftest();

   void reset( HASH_FUNCTION _type );
};
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
