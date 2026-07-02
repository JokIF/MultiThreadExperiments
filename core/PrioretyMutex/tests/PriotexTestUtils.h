#pragma once
#ifdef BUILD_TESTING
#include "Priotex.h"

namespace unitTests
{
class UnitTestDataPriotex
{
public:
    UnitTestDataPriotex(const mutex::Priotex& ptx) : ptx(ptx) {}

    mutex::Priotex::Priorety GetPriotexPriorety() const { return ptx.m_prioretyValue; }
    mutex::Priotex::Priorety GetLastPriorety() const { return ptx.m_prevPrioretyValue; }
    mutex::Priotex::Priorety GetCurrentThreadPriorety() const { return ptx.m_thisThreadPrioretyValue; }

private:
    const mutex::Priotex& ptx;
};
}

//---------------------------------------------------- To Other SUBDIR-------------------------------------------------------------
#include <thread>
#include <functional>
#include <memory>

namespace threadWorks
{
template <typename Thread> //Need concept
class SignalThreadWrapper
{
    using thread_type = std::remove_cv_t<std::remove_reference_t<Thread>>;

public:
    constexpr explicit SignalThreadWrapper() : mainThread(std::make_unique<thread_type>()) {}
    ~SignalThreadWrapper() { if(this->joinable()) this->join(); }

    template<typename F, typename... Args>
    explicit SignalThreadWrapper(F&& initFunc, Args&&... args) 
        : mainThread(std::make_unique<thread_type>(std::forward<F>(initFunc), std::forward<Args>(args)...)) {}
    explicit SignalThreadWrapper(thread_type&& outerThread) : mainThread(std::make_unique<thread_type>(std::move(outerThread))) {}
    
    SignalThreadWrapper(const SignalThreadWrapper&) = delete;
    SignalThreadWrapper& operator=(const SignalThreadWrapper&) = delete;
    
    SignalThreadWrapper(SignalThreadWrapper&&) = default;
    SignalThreadWrapper& operator=(SignalThreadWrapper&&) = delete;

    bool joinable() const noexcept { return mainThread->joinable(); }
    void detach() { mainThread->detach(); }
    auto get_id() const { return mainThread->get_id(); }

    void join()
    {
        mainThread->join();
        onJoinCallback();
    }

private:
    template <typename Callback>
    void SetOnJoinCallback(Callback&& callback) { onJoinCallback = std::forward<Callback>(callback); }

    std::unique_ptr<thread_type> mainThread;
    std::function<void(void)> onJoinCallback;

    template <typename ThreadType, typename Callback, typename F, typename... Args>
    friend SignalThreadWrapper<ThreadType> make_signal_thread_wrapper(Callback&& callback, F&& initFunc, Args&&... args);
};

template <typename Thread, typename Callback, typename F, typename... Args>
SignalThreadWrapper<Thread> make_signal_thread_wrapper(Callback&& callback, F&& initFunc, Args&&... args)
{
    SignalThreadWrapper<Thread> sharedThread(std::forward<F>(initFunc), std::forward<Args>(args)...);
    sharedThread.SetOnJoinCallback(std::forward<Callback>(callback));
    return sharedThread;
}

class ThreadLauncher
{
public:
    explicit ThreadLauncher() = default;

    template<typename F, typename... Args>
    auto CreateThread(F&& func, Args&&... args) 
    {
        auto thread = make_signal_thread_wrapper<std::thread>([this]{ this->threadCount--; }, std::forward<F>(func), std::forward<Args>(args)...);
        threadCount++;
        return thread;
    }

    short GetThreadCount() const { return threadCount.load(); }

private:

    std::atomic<short> threadCount = 0;
};
}
//---------------------------------------------------- To Other SUBDIR-------------------------------------------------------------


#endif