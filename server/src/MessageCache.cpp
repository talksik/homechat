#include "include/MessageCache.hpp"

bool MessageCache::add_message(std::string message)
{
    if (m_messages.size() == m_cache_size)
    {
        m_messages.erase(m_messages.begin());
    }
    m_messages.push_back(message);
    return true;
}

MessageCache::MessageCache()
{
}

bool MessageCache::empty()
{
    return m_messages.empty();
}

std::vector<std::string> MessageCache::last_ten_messages() const
{
    return m_messages.size() > 10 ? std::vector<std::string>(m_messages.end() - 10, m_messages.end()) : m_messages;
}
