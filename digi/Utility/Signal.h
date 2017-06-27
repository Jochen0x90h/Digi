#ifndef digi_Utility_Signal_h
#define digi_Utility_Signal_h

#include <boost/signals2.hpp>


namespace digi {

/// @addtogroup Utility
/// @{

// signal with no mutex
template <typename F>
class Signal : public boost::signals2::signal_type<F, boost::signals2::keywords::mutex_type<boost::signals2::dummy_mutex> >::type
{
};

// signal with mutex to syncronize calls from multiple threads
template <typename F>
class SynchronizedSignal : public boost::signals2::signal<F>
{
};

// track deletion of objects that are the target of a signal
typedef boost::signals2::trackable Trackable;

} // namespace digi

#endif
