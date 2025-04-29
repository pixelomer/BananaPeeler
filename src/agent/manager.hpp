#pragma once

#include "../acs/header.hpp"
#include "animation.hpp"
#include "environment.hpp"
#include <memory>

/*

ANIMATION EXITING (TRANSITION TYPES)
- None: animations ends in a neutral state
- Exit Branching: animation can be interrupted by following exit branches
                  the agent will be in a neutral state afterwards
- Return: agent will NOT be in a neutral state after animating, return
          animation must be manually played afterwards

*/

namespace banana {
namespace agent {

class manager {
private:
    std::shared_ptr<acs::header> m_acsHeader;
    std::shared_ptr<agent::environment> m_env;
    animation m_animation;
public:
    std::shared_ptr<acs::header> acsHeader() {
        return m_acsHeader;
    }
    void setAcsHeader(std::shared_ptr<acs::header> acsHeader) {
        m_acsHeader = acsHeader;
    }
    std::shared_ptr<agent::environment> environment() {
        return m_env;
    }
    void setEnvironment(std::shared_ptr<agent::environment> env) {
        m_env = env;
    }

    manager(std::shared_ptr<acs::header> acsHeader): m_acsHeader(acsHeader) {}
    manager() {}

    void play(std::string const& animationName) {
        acs::animation_info *next = nullptr;
        if (m_animation.current()->transition_type == acs::transition_type::UseReturnAnimation) {
            m_animation = { &m_acsHeader->animation_named(animationName) };
        }
        else if (m_animation.queued())
    }

    void speak(std::string const& text) {
        
    }
};

}
}
