---
title: Concurrency
tagline: No Raw Synchronization Primitives
mathjax: true
layout: book-page
tags: [ better-code ]
---

### Motivation

{::comment}
For this section I need to first provide motivation for concurrency, and define concurrency and parallelism. which are not commonly understood. Do I need to provide a motivation section for each chapter?
{:/comment}

_Concurrency_ is when multiple tasks start, run, and complete in overlapping time periods and should not be confused with _parallelism_ which is when multiple tasks execute simultaneously. Parallelism requires some form of hardware support, where as concurrency can be achieved strictly through software, such as a cooperative tasking system.

There are two primary benefits for concurrent code. The first is performance by enabling parallelism. The second is to improve interactivity by not blocking the user while a prior action is being processed.

As clock rates on systems have stagnated, hardware developers have turned to parallelism to increase performance. Figure [xxx] shows the performance distribution on a typical desktop system. A single threaded, non-vectorized, application can only utilize about 0.25% of the performance capabilities of the machine.

### Definition of _raw synchronization primitives_.

A _raw synchronization primitive_ is a low level construct used to synchronize access to data. Examples include locks and mutexes, condition variables, semaphores, atomic operations, and memory fences.

{::comment} Discuss difference between data parallelism and task concurrency, so far this chapter is only dealing with tasking. However, it could be expanded upon. {:/comment}

The goal of this chapter is to develop concurrent code without using raw synchronization primitives.

The first problem with raw synchronization primitives are that they are exceedingly error prone to use because, by definition, they require reasoning about non-local effects.

For example, the following is a snippet from a copy-on-write datatype, this is a simplified version of code from a shipping system. 

~~~c++
template <typename T>
class bad_cow {
    struct object_t {
        explicit object_t(const T& x) : data_m(x) {}
        atomic<int> count_m{1};
        T           data_m; };
    object_t* object_m;
 public:
    explicit bad_cow(const T& x) : object_m(new object_t(x)) { }
    ~bad_cow() { if (0 == --object_m->count_m) delete object_m; }
    bad_cow(const bad_cow& x) : object_m(x.object_m) { ++object_m->count_m; }

    bad_cow& operator=(const T& x) {
        if (object_m->count_m == 1) object_m->data_m = x;     // position 1
        else {
            object_t* tmp = new object_t(x);
            --object_m->count_m;                              // position 2
            object_m = tmp;
        }
        return *this;
    }
};
~~~

The highlighted lines {::comment} how? {:/comment} contain a subtle race condition. The `if` statement at _position 1_ is checking the value of an atomic count to see if it is `1`. The `else` statement handles the case where it is not 1. Within the else statement the count is decremented at _position 2_. The problem is that if decrementing the count results in a value of `0` then the object stored in `object_m` should be deleted. The code fails to check for this case, and so an object may be leaked.

The initial test to see if the count was `1` isn't sufficient, between that check and when the count is decremented another thread may have released ownership and decremented the count leaving this object instance as the sole owner.

The fix is to test atomically with the decrement in the same statement, see _position 3_. The correct code is shown in shown below:

~~~c++
template <typename T>
class correct_cow {
    struct object_t {
        explicit object_t(const T& x) : data_m(x) {}
        atomic<int> count_m{1};
        T           data_m; };
    object_t* object_m;
 public:
    explicit correct_cow(const T& x) : object_m(new object_t(x)) { }
    ~correct_cow() { if (0 == --object_m->count_m) delete object_m; }
    correct_cow(const correct_cow& x) : object_m(x.object_m) { ++object_m->count_m; }

    correct_cow& operator=(const T& x) {
        if (object_m->count_m == 1) object_m->data_m = x;
        else {
            object_t* tmp = new object_t(x);
            if (0 == --object_m->count_m) delete object_m;    // position 3
            object_m = tmp;
        }
        return *this;
    }
};
~~~

The code of the complete implementations is here: [https://github.com/stlab/libraries/blob/develop/stlab/copy_on_write.hpp](https://github.com/stlab/libraries/blob/develop/stlab/copy_on_write.hpp)

Another problem with raw synchronization primitives is that their use can have a large negative impact on system performance. To understand why, we need to understand Amdahl's Law.

The intuition behind Amdahl's Law is that if a part of system takes time x to complete on a single core or processor, then it will encounter a speedup of y if it is run on y cores, but only if no synchronization takes places between the different cores. This speedup is in most of the cases an ideal that is never reachable because even if no synchronization is expressed in the program code, there are synchronization mechanisms on the chip or the board, like memory bus, etc.

$$ S(N) = \frac{1}{(1-P)+\frac{P}{N}} $$
Where the speedup $$S$$ is defined by this equation. $$P$$ is hereby the amount of synchronization in the range of $$[0 .. 1]$$ and $$N$$ the number of cores or processors.

Drawing the abscissa in logarithmic scale illustrates that there is only a speedup of 20 when the system is running on 2048 cores or more and just 5% synchronization takes place.

![Amdahl's Law](figures/amdahl_log.png) 

Amdahl's Law Logarithmic Scale

![Amdahl's Law](figures/amdahl_lin.png) 

Amdahl's Law Linear Scale

{::comment}
Math experiment for Fibonacci matrix.
{:/comment}
$$
\begin{align*}
  \left[ \begin{array}{cc}
      1 & 1 \\
      1 & 0
    \end{array} \right]^{n} =
    \left[ \begin{array}{cc}
      F_{n+1} & F_n \\
      F_n & F_{n-1}
    \end{array} \right]
\end{align*}
$$

### Develop Solution

### Conclusion
