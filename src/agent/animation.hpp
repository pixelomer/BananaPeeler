#pragma once

#include "../acs/animation_info.hpp"

//FIXME: this is impractical since it can't directly access the acs header

namespace banana {
namespace agent {

class animation {
private:
    int m_frame;
    acs::animation_info *m_current;
    acs::animation_info *m_queued;
public:
    animation(acs::animation_info *current = nullptr,
        acs::animation_info *queued = nullptr):
        m_frame(0), m_current(current), m_queued(queued) {}

    acs::animation_info *current() {
        return m_current;
    }

    acs::animation_info *queued() {
        return m_queued;
    }

    acs::frame_info &activeFrame() {
        return m_current->frames[m_frame];
    }

    bool canSpeak() {
        return m_current->frames[m_frame].overlays.size() > 0;
    }

    bool done() {
        return m_frame >= m_current->frames.size();
    }

    void advance() {
        if (done()) {
            return;
        }
        ++m_frame;
        if (done() && m_queued != nullptr) {
            *this = { m_queued };
        }
    }

    bool valid() {
        return m_current != nullptr;
    }
};

}
}
