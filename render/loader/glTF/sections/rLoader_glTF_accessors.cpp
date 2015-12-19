/*!
 * \file rLoader_glTF_accessors.hpp
 * \brief \b Classes: \a rLoader_glTF
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

#include "rLoader_glTF.hpp"

namespace e_engine {

bool rLoader_glTF::sectionAccessors() {
   if ( !expect( '{' ) )
      return false;

   ELEMENTS lSection;
   std::string lName;
   size_t lID;

   while ( vIter != vEnd ) {
      lName.clear();
      if ( !getString( lName ) )
         return false;

      if ( !expect( ':' ) )
         return false;

      if ( !expect( '{' ) )
         return false;

      lID = getItem( vAccessors, vAccessorMap, lName );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            case NAME:
               if ( !getString( vAccessors[lID].name ) )
                  return false;

               break;
            case BUFFERVIEW:
               if ( !getString( lName ) )
                  return false;

               vAccessors[lID].bufferView = getItem( vBufferViews, vBufferViewMap, lName );
               break;
            case BYTEOFFSET:
               if ( !getNum( vAccessors[lID].byteOffset ) )
                  return false;

               break;
            case BYTESTRIDE:
               if ( !getNum( vAccessors[lID].byteStride ) )
                  return false;

               break;
            case COMPONENTTYPE:
               if ( !getMapElementETC( vAccessors[lID].componentType ) )
                  return false;

               break;
            case COUNT:
               if ( !getNum( vAccessors[lID].count ) )
                  return false;

               break;
            case TYPE:
               if ( !getMapElement( vAccessors[lID].type, false ) )
                  return false;

               break;
            case MAX:
            case MIN:
            case EXTENSIONS:
            case EXTRAS:
               if ( !skipSection() )
                  return false;

               break;
            default: return wrongKeyWordError();
         }

         if ( expect( ',', true, true ) )
            continue;

         if ( expect( '}', false ) )
            break;

         return unexpectedCharError();
      }

#if D_LOG_GLTF
      vAccessors[lID].print( this );
#endif

      if ( expect( ',', true, true ) )
         continue;

      if ( expect( '}', false ) )
         break;

      return unexpectedCharError();
   }

   return true;
}
}
