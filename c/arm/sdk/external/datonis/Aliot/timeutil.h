/*
 * Declares time related functions that are to be implemented by a System Integrator
 * based on the underlying hardware
 *
 * Rajesh Jangam (Altizon Systems Pvt. Ltd.)
 */

#ifndef TIME_H
#define TIME_H 1

unsigned long long get_time_ms();

void sleep_seconds(unsigned int seconds);

#endif

