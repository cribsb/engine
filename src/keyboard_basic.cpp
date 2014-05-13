/*!
 * \file keyboard_basic.hpp
 *
 * Basic class for setting keys
 */

#include "keyboard_basic.hpp"

namespace e_engine {

eKeyboardBasic::eKeyboardBasic()  {
   for ( unsigned short int i = 0; i < ( _E_KEY_LAST + 1 ); i++ )
      key_state[i] = E_KEY_RELEASED;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 