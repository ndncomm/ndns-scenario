#include "mgmt/management-tool.hpp"
#include "ndns-label.hpp"
#include "logger.hpp"
#include "util/util.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>

namespace ndn {
namespace ndns {

class NdnsAddRR
{
public:
NdnsAddRR(std::string zoneStr, std::string rrLabelStr, std::string ndnsTypeStr,
          std::string rrTypeStr, std::vector<std::string> content, std::string db, ndn::Name dsk)
  : m_ttlInt(-1)
  , m_versionInt(-1)
  , m_zoneStr(zoneStr)
  , m_rrLabelStr(rrLabelStr)
  , m_ndnsTypeStr(ndnsTypeStr)
  , m_rrTypeStr(rrTypeStr)
  , m_content(content)
  , m_db(db)
  , m_dsk(dsk)
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

void
setndnsTypeStr(std::string& ndnsTypeStr) {
  m_ndnsTypeStr = ndnsTypeStr;
}

std::string
getndnsTypeStr() const {
  return m_ndnsTypeStr;
}

void
setContent(std::vector<std::string>& content) {
  m_content = content;
}

std::vector<std::string>
getContent() const {
  return m_content;
}

void
setTtlInt(int& ttlInt) {
  m_ttlInt = ttlInt;
}

int
getTtlInt() const {
  return m_ttlInt;
}

void
setVersionInt(int& versionInt) {
  m_versionInt = versionInt;
}

int
getVersionInt() const {
  return m_versionInt;
}

std::string
getDb() const {
  return m_db;
}

void
setDb(std::string& db) {
  m_db = db;
}

ndn::Name
getDsk() const {
  return m_dsk;
}

void
setDsk(ndn::Name& dsk) {
  m_dsk = dsk;
}

void
addRR() {
  ndn::Name zoneName(m_zoneStr);
  ndn::Name label(m_rrLabelStr);
  ndn::Name::Component type(m_rrTypeStr);
  ndn::ndns::NdnsType ndnsType = ndns::toNdnsType(m_ndnsTypeStr);
  time::seconds ttl;

  if (m_ttlInt == -1)
    ttl = ndn::ndns::DEFAULT_CACHE_TTL;
  else
    ttl = time::seconds(m_ttlInt);

  uint64_t version = static_cast<uint64_t>(m_versionInt);

  ndn::KeyChain keyChain;
  ndn::ndns::ManagementTool tool(m_db, keyChain);
  tool.addRrSet(zoneName, label, type, ndnsType, version, m_content, m_dsk, ttl);
}


private:
int m_ttlInt;
int m_versionInt;
std::string m_zoneStr;
std::string m_rrLabelStr;
std::string m_ndnsTypeStr;
std::string m_rrTypeStr;
std::vector<std::string> m_content;
std::string m_db;
ndn::Name m_dsk;
};

} // namespace ndns
} // namespace ndn