/*
Module for measuring communication latency and clock offset between Jetson (ROS2)
and PX4 using a ping-pong timestamp exchange.

Jetson periodically sends a PING message (with Jetson timestamp T1).
PX4 receives it at time T2 (PX4 clock), immediately responds with a PONG
message containing T1 and its own send time T3. Jetson receives the PONG at T4.

The times of events are defined as follows:
T1 — Jetson sends PING      (Jetson clock)
T2 — PX4 receives PING      (PX4 clock)
T3 — PX4 sends PONG         (PX4 clock)
T4 — Jetson receives PONG   (Jetson clock)

T1 -> (network delay) -> T2
T2 -> (PX4 processing tiny) -> T3
T3 -> (network delay) -> T4
*/

#include <cstdint>
#include "sync_timers.hpp"
#include <px4_platform_common/module.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/time.h>

int SyncTimers::run()
{
	PX4_INFO("sync_timers started");

	pingPongLoop();

	PX4_INFO("sync_timers exited");
	return 0;
}

void SyncTimers::pingPongLoop()
{
    using namespace time_literals;

    while (true) {

        latency_ping_s ping{};

        if (_ping_sub.update(&ping)) {	// T2: PX4 receives PING

            // Check for exit signal
            if (ping.timestamp_t1 == UINT64_MAX) {
                PX4_INFO("Recived close signal. Exiting...");
                break;
            }

            uint64_t T1 = ping.timestamp_t1;	// in microseconds (us)

            uint64_t T3 = hrt_absolute_time();	// in microseconds (us)

            latency_pong_s pong{};
            pong.timestamp_t1 = T1;
            pong.timestamp_t3 = T3;

            _pong_pub.publish(pong);
        }

        px4_usleep(100_us);
    }
}

extern "C" __EXPORT int sync_timers_main(int argc, char *argv[])
{
    SyncTimers app;
    return app.run();
}
