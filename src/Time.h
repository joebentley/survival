#ifndef TIME_H_
#define TIME_H_

struct Time {
    int mHour;
    int mMinute;

    Time() : mHour(0), mMinute(0) {}
    Time(int hour, int minute) : mHour(hour), mMinute(minute) {}

    bool operator==(const Time &rhs) const {
        return mHour == rhs.mHour &&
               mMinute == rhs.mMinute;
    }

    bool operator!=(const Time &rhs) const {
        return !(rhs == *this);
    }

    Time& operator+=(const Time& rhs) {
        mHour += (mMinute + rhs.mMinute) / 60;
        mMinute = (mMinute + rhs.mMinute) % 60;
        mHour += rhs.mHour;
        mHour %= 24;

        return *this;
    }

    std::string toString() const {
        return (mHour < 10 ? "0" : "") + std::to_string(mHour) + ":" + (mMinute < 10 ? "0" : "") + std::to_string(mMinute);
    }

    std::string toWordString() const {
        if (mHour >= 21 || mHour < 5)
            return "Night";
        else if (mHour < 8)
            return "Dawn";
        else if (mHour < 16)
            return "Day";
        else if (mHour < 21)
            return "Dusk";
        return "";
    }

    float getFractionOfDay() const {
        return static_cast<float>(mHour * 60 + mMinute) / (60 * 24);
    }
};

inline Time operator+(Time lhs, const Time& rhs) {
    lhs += rhs;
    return lhs;
}

#endif // TIME_H_