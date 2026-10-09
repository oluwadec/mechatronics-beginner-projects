#!/usr/bin/env python3
"""
Plot step-response data logged from Project 01 (DC Motor PID).

Usage:
    python plot_step_response.py data.csv

Expected CSV columns (no header required, or with header):
    time_ms, setpoint_deg, position_deg, error, pwm
"""

import sys
import numpy as np
import matplotlib.pyplot as plt

def load_data(filename):
    data = np.genfromtxt(filename, delimiter=',', names=True,
                         dtype=None, encoding=None)
    # Handle both named and positional
    if data.dtype.names is None:
        t = data[:, 0] / 1000.0
        sp = data[:, 1]
        pos = data[:, 2]
    else:
        t = data['time_ms'] / 1000.0 if 'time_ms' in data.dtype.names else data[data.dtype.names[0]] / 1000.0
        sp = data['setpoint_deg'] if 'setpoint_deg' in data.dtype.names else data[data.dtype.names[1]]
        pos = data['position_deg'] if 'position_deg' in data.dtype.names else data[data.dtype.names[2]]
    t = t - t[0]   # start at zero
    return t, sp, pos

def metrics(t, sp, pos):
    final = sp[-1]
    # Rise time 10–90 %
    low = final * 0.1
    high = final * 0.9
    try:
        t10 = t[np.where(pos >= low)[0][0]]
        t90 = t[np.where(pos >= high)[0][0]]
        rise = t90 - t10
    except IndexError:
        rise = np.nan

    # Overshoot
    peak = np.max(pos)
    overshoot = (peak - final) / abs(final) * 100 if final != 0 else 0

    # Steady-state error (last 20 % of data)
    n = max(5, len(pos) // 5)
    ss_err = np.mean(pos[-n:] - final)

    # Settling time (±5 %)
    tol = abs(final) * 0.05
    settled = np.where(np.abs(pos - final) > tol)[0]
    settle = t[settled[-1]] if len(settled) > 0 else 0.0

    return rise, overshoot, ss_err, settle

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_step_response.py <logfile.csv>")
        sys.exit(1)

    t, sp, pos = load_data(sys.argv[1])
    rise, overshoot, ss_err, settle = metrics(t, sp, pos)

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(t, sp, 'k--', label='Setpoint')
    ax.plot(t, pos, 'b-', label='Position')
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Angle (deg)')
    ax.set_title('Step Response — DC Motor Position PID')
    ax.grid(True)
    ax.legend()

    text = (f"Rise time (10–90%): {rise:.3f} s\n"
            f"Overshoot: {overshoot:.1f} %\n"
            f"Steady-state error: {ss_err:.2f} °\n"
            f"Settling time (±5%): {settle:.3f} s")
    ax.text(0.98, 0.02, text, transform=ax.transAxes,
            verticalalignment='bottom', horizontalalignment='right',
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

    plt.tight_layout()
    outname = sys.argv[1].rsplit('.', 1)[0] + '_plot.png'
    plt.savefig(outname, dpi=150)
    print(f"Saved plot to {outname}")
    print(text)
    plt.show()

if __name__ == '__main__':
    main()
