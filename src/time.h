#ifndef TIME_H_
#define TIME_H_

struct Time {
    int hour;
    int minute;

    Time() : hour(0), minute(0) {}
    Time(int hour, int minute) : hour(hour), minute(minute) {}

    bool operator==(const Time &rhs) const {
        return hour == rhs.hour &&
               minute == rhs.minute;
    }

    bool operator!=(const Time &rhs) const {
        return !(rhs == *this);
    }

    Time& operator+=(const Time& rhs) {
        hour += (minute + rhs.minute) / 60;
        minute = (minute + rhs.minute) % 60;
        hour += rhs.hour;
        hour %= 24;

        return *this;
    }

    std::string toString() const {
        return (hour < 10 ? "0" : "") + std::to_string(hour) + ":" + (minute < 10 ? "0" : "") + std::to_string(minute);
    }

    std::string toWordString() const {
        if (hour >= 21 || hour < 5)
            return "Night";
        else if (hour < 8)
            return "Dawn";
        else if (hour < 16)
            return "Day";
        else if (hour < 21)
            return "Dusk";
        return "";
    }

    float getFractionOfDay() const {
        return static_cast<float>(hour * 60 + minute) / (60 * 24);
    }
};

inline Time operator+(Time lhs, const Time& rhs) {
    lhs += rhs;
    return lhs;
}

#endif // TIME_H_