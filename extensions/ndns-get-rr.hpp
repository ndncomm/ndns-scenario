#include "mgmt/management-tool.hpp"
#include "ndns-label.hpp"
#include "logger.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace ndn {
namespace ndns {

class NdnsGetRR {
public:
  NdnsGetRR (std::string& zoneStr, std::string& db,
             std::string& rrLabelStr, std::string& rrTypeStr)
    : m_zoneStr(zoneStr)
    , m_db(db)
    , m_rrLabelStr(rrLabelStr)
    , m_rrTypeStr(rrTypeStr)
    {
    }

  void
  setZoneStr(std::string& zoneStr) {
    m_zoneStr = zoneStr;
  }

  std::string
  getZoneStr() const {
    return m_zoneStr;
  }

  void
  setrrLabelStr(std::string& rrLabelStr) {
    m_rrLabelStr = rrLabelStr;
  }

  std::string
  getrrLabelStr() const {
    return m_rrLabelStr;
  }

  std::string
  getDb() const {
    return m_db;
  }

  void
  setDb(std::string& db) {
    m_db = db;
  }

  std::string
  getRrTypeStr() const {
    return m_rrTypeStr;
  }

  void
  setRrTypeStr(std::string rrTypeStr) {
    m_rrTypeStr = rrTypeStr;
  }

  void
  getRr() {
    Name zoneName(m_zoneStr);
    Name label(m_rrLabelStr);
    name::Component type(m_rrTypeStr);

    ndn::KeyChain keyChain;
    ndn::ndns::ManagementTool tool(m_db, keyChain);
    tool.getRrSet(zoneName, label, type, std::cout);
  }


private:
  std::string m_zoneStr;
  std::string m_db;
  std::string m_rrLabelStr;
  std::string m_rrTypeStr;
};

} // namespace ndns
} // namespace ndn