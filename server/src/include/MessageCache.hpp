#include <string>
#include <vector>

class MessageCache
{
  public:
    MessageCache();

    // adds and removes the oldest message if the cache is full
    bool add_message(std::string message);

    bool empty();
    std::vector<std::string> last_ten_messages() const;

  private:
    std::vector<std::string> m_messages;
    static constexpr int m_cache_size = 100;
};
