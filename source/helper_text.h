#ifdef _MSC_VER
#define aflog printf
#define snprintf sprintf_s
#define vsnprintf vsprintf_s
#define stricmp _stricmp
#else
#define strcpy_s(dst,size,src) strlcpy(dst,src,size)
#define strcat_s(dst,size,src) strlcat(dst,src,size)
#define stricmp strcasecmp
#define aflog(...) __android_log_print(ANDROID_LOG_DEBUG, "MyTag", __VA_ARGS__)
#endif

#ifdef _MSC_VER
#define CP_SHIFT_JIS 932
template < int BUF_LEN, int CODEPAGE > class TUnicodeToAnsi{
	char m_buf[ BUF_LEN ];
  public:
	TUnicodeToAnsi( const wchar_t *text ){
		memset( this, 0, sizeof*this );
		WideCharToMultiByte( CODEPAGE, 0, text, -1, m_buf, BUF_LEN - 1, NULL, NULL );
	}
	operator char*(){ return m_buf; }
};

typedef TUnicodeToAnsi<512,CP_ACP> UnicodeToAnsi;
typedef TUnicodeToAnsi<512,CP_SHIFT_JIS> UnicodeToSjis;

template < int BUF_LEN, int CODEPAGE > class TAnsiToUnicode{
	WCHAR m_buf[ BUF_LEN ];
  public:
	TAnsiToUnicode( const char *text ){
		memset( this, 0, sizeof*this );
		MultiByteToWideChar( CP_SJIS, 0, text, -1, m_buf, BUF_LEN - 1 );
	}
	operator WCHAR*(){ return m_buf; }
};

typedef TAnsiToUnicode<512,CP_ACP> AnsiToUnicode;
typedef TAnsiToUnicode<512,CP_SHIFT_JIS> SjisToUnicode;
#endif

template <int BUF_LEN> class TSWPrintf{
	wchar_t m_buf[ BUF_LEN ];
  public:
	TSWPrintf( const wchar_t *format, ... ){
		memset( this, 0, sizeof*this );
		va_list argptr;
		va_start( argptr, format );
		vswprintf( m_buf, format, argptr );
		va_end( argptr );
	}

	TSWPrintf( const wchar_t *format, va_list argptr ){
		memset( this, 0, sizeof*this );
		vswprintf( m_buf, format, argptr );
	}

	operator wchar_t*(){ return m_buf; }
};
typedef TSWPrintf<512> SWPrintf;

template <int BUF_LEN> class TExtractLineText{
	wchar_t m_buf[ BUF_LEN ];
  public:
	TExtractLineText( const wchar_t *text, int line ){
		memset( this, 0, sizeof*this );
		wchar_t *t = (wchar_t*)text;
		while( line-- > 0 ){
			wchar_t *p = wcschr( t, '\n' );
			if( !p ) return;
			t = p + 1;
		}

		for( int i = 0; i < BUF_LEN - 1; i++ ){
			if( wcschr( L"\r\n", t[ i ]  ) )
				break;
			m_buf[ i ] = t[ i ];
		}
	}
	operator wchar_t*(){ return m_buf; }
};
typedef TExtractLineText<512> ExtractLineText;


template <int BUF_LEN> class TSPrintf{
	char m_buf[BUF_LEN];
public:
	TSPrintf(const char *format, ...){
		memset(this, 0, sizeof*this);
		va_list argptr;
		va_start(argptr, format);
		vsnprintf(m_buf, sizeof(m_buf), format, argptr);
		va_end(argptr);
	}

	operator char*(){ return m_buf; }
};
typedef TSPrintf<512> SPrintf;
