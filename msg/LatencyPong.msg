# PX4 sends this back to Jetson
# T1: Jetson's time when PING was sent (mirrored back)
# T3: PX4's time when PONG was sent (in microseconds)

uint64 timestamp			# time since system start (microseconds)
uint64 timestamp_t1
uint64 timestamp_t3
