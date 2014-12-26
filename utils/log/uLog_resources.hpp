/*!
 * \file uLog_resources.hpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
 */

#ifndef E_LOG_STRUCTS_HPP
#define E_LOG_STRUCTS_HPP

#include "defines.hpp"

#include <thread>
#include <condition_variable>
#include <tuple>
#include "uSignalSlot.hpp"
#include "uLog_converters.hpp"
#include "uConfig.hpp"  // Only for internal::LOG_COLOR_TYPE and internal::LOG_PRINT_TYPE


namespace e_engine {

class uLogEntryRaw;

namespace internal {

enum LOG_OBJECT_TYPE { STRING, NEW_LINE, NEW_POINT };


/*!
 * \struct e_engine::internal::uLogType
 * \brief Holds information about a (new) output type for class \c uLog
 *
 * This structure defines all variables which are important
 * for a (new) output type for the class \c uLog
 *
 * \sa uLog
 */
class uLogType {
      typedef uSignal<void, uLogEntryRaw &> _SIGNAL_;
   private:
      char         vType_C;       //!< The character witch is associated with color and output mode
      std::wstring vType_STR;
      char         vColor_C;      //!< The ID from struct \c eCMDColor for the color which should be used
      bool         vBold_B;


      _SIGNAL_     vSignal_eSIG;  //!< \warning The connections will never copy!

      uLogType() {}
   public:
      uLogType( char _type, std::wstring _typeString, char _color, bool _bold )
         : vType_C( _type ), vType_STR( _typeString ), vColor_C( _color ), vBold_B( _bold ) {}

      inline char         getType()   const { return vType_C; }
      inline std::wstring getString() const { return vType_STR; }
      inline char         getColor()  const { return vColor_C; }
      inline bool         getBold()   const { return vBold_B; }

      inline _SIGNAL_   *getSignal() { return &vSignal_eSIG; }

      //void send( uLogEntryStruct _data )   { vSignal_eSIG( _data ); }
};

template<class __A, class... __T>
struct uConverter {
   static void convert( std::wstring &_str, __A&&_toConvert, __T&&... _rest ) {
      uLogConverter<__A>::convert( _str, std::forward<__A>( _toConvert ) );
      uConverter<__T...>::convert( _str, std::forward<__T>( _rest )... );
   }
};

template<class __A>
struct uConverter<__A> {
   static void convert( std::wstring &_str, __A&& _toConvert ) {
      uLogConverter<__A>::convert( _str, std::forward<__A>( _toConvert ) );
   }
};


/*!
 * \brief Contains raw data for a log entry
 *
 * Stores the raw input data with boost::variant and uses some
 * template metaprogramming.
 */
struct uLogRawData {
   virtual std::wstring get() = 0;
   virtual ~uLogRawData() {}
};

template<class... T>
struct uLogRawDataT : uLogRawData {
   std::tuple < T... > vData;

   uLogRawDataT( T&&... _d ) : vData( std::forward<T>(_d)... ) {}

   template<int... sequenze>
   void get( std::wstring &_str, templates::intSequenze<sequenze...> ) {
      uConverter<T...>::convert( _str, std::forward<T>( std::get<sequenze>( vData ) )... );
   }

   std::wstring get() {
      std::wstring lTempStr;
      get( lTempStr, templates::makeIntSequenze<sizeof...(T)>{} );
      return lTempStr;
   }

   virtual ~uLogRawDataT() {}
};
}

class uLog;

class uLogEntryRaw {
   public:
      struct __DATA__ {
         std::wstring vResultString_STR;

         struct __DATA_RAW__ {
            std::wstring vDataString_STR;
            std::wstring vFilename_STR;
            std::wstring vFunctionName_STR;
            std::wstring vType_STR;
            char         vBasicColor_C;
            bool         vBold_B;
            int          vLine_I;
            std::time_t  vTime_lI;

            __DATA_RAW__( std::wstring &_filename, int &_line, std::wstring &_funcName ) :
               vFilename_STR( _filename ),
               vFunctionName_STR( _funcName ),
               vLine_I( _line ) {

               std::time( &vTime_lI );
            }
         } raw;

         struct __DATA_CONF__ {
            LOG_COLOR_TYPE vColor_LCT;
            LOG_PRINT_TYPE vTime_LPT;
            LOG_PRINT_TYPE vFile_LPT;
            LOG_PRINT_TYPE vErrorType_LPT;
            int            vColumns_uI;
            uint16_t       vMaxTypeStringLength_usI;
            bool           vTextOnly_B;
            __DATA_CONF__( bool &_onlyText ) : vTextOnly_B( _onlyText ) {}
         } config;

         void configure( e_engine::LOG_COLOR_TYPE _color, e_engine::LOG_PRINT_TYPE _time, e_engine::LOG_PRINT_TYPE _file, e_engine::LOG_PRINT_TYPE _errorType, int _columns );

         __DATA__( std::wstring && _filename, int &_line, std::wstring && _funcName, bool &_textOnly ) :
            raw(
                  _filename,
                  _line,
                  _funcName
            ),
            config(
                  _textOnly
            )
         {}
      } data;

   private:
      bool                    vComplete_B;
      internal::uLogRawData  *vElements = nullptr;
      char                    vType_C;

      std::condition_variable vWaitUntilThisIsPrinted_BT;
      std::mutex              vWaitMutex_BT;
      bool                    vIsPrinted_B;

      std::condition_variable vWaitUntilEndFinisched_BT;
      std::mutex              vWaitEndMutex_BT;
      bool                    vEndFinished_B;

      const unsigned int      vSize;

      void end();
      void endLogWaitAndSetPrinted();

   public:
      template<class... ARGS>
      uLogEntryRaw( char _type, bool _onlyText, std::string _rawFilename, int _logLine, std::string _functionName, ARGS&&... _args ) :
         data(
               std::wstring( _rawFilename.begin(), _rawFilename.end() ),
               _logLine,
               std::wstring( _functionName.begin(), _functionName.end() ),
               _onlyText
         ),
         vComplete_B( false ),
         vType_C( _type ),
         vIsPrinted_B( false ),
         vEndFinished_B( false ),
         vSize( sizeof...( ARGS ) ) {

         vElements = new internal::uLogRawDataT<ARGS...>( std::forward<ARGS>(_args)... );
         vComplete_B = true;
      }

      uLogEntryRaw() = delete;

      ~uLogEntryRaw();

      inline bool   getIsComplete()   const   { return vComplete_B; }
      inline bool   getIsPrinted()    const   { return vIsPrinted_B; }
      inline size_t getElementsSize() const   { return vSize; }
      unsigned int  getLogEntry( std::vector< e_engine::internal::uLogType > &_vLogTypes_V_eLT );

      void          defaultEntryGenerator();

      friend class uLog;
};



}



#endif //E_LOG_STRUCTS_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

