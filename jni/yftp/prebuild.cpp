#define BOOST_ALL_NO_LIB

#include <boost/filesystem.hpp>
#include <libs/system/src/error_code.cpp>
#include <libs/filesystem/src/codecvt_error_category.cpp>
#include <libs/filesystem/src/operations.cpp>
#include <libs/filesystem/src/path_traits.cpp>
#include <libs/filesystem/src/path.cpp>
#include <libs/filesystem/src/portability.cpp>
#include <libs/filesystem/src/unique_path.cpp>
#include <libs/filesystem/src/utf8_codecvt_facet.cpp>
#include <libs/filesystem/src/windows_file_codecvt.cpp>

#include <boost/thread.hpp>
#ifdef _MSC_VER
namespace boost {
	void tss_cleanup_implemented(void) {}
}
#include <libs/thread/src/win32/thread.cpp>
#include <libs/thread/src/win32/tss_dll.cpp>
#include <libs/thread/src/win32/tss_pe.cpp>

#else
#include <libs/thread/src/pthread/thread.cpp>
#include <libs/thread/src/pthread/once.cpp>
//#include <libs/thread/src/pthread/once_atomic.cpp>

#endif

#include <boost/atomic.hpp>
#include <libs/atomic/src/lockpool.cpp>

#include <boost/chrono.hpp>
#include <libs/chrono/src/chrono.cpp>