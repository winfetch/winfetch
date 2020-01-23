#include "pch.h"

#include <filesystem>
#include <optional>

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Web::Http::Diagnostics;
using namespace Windows::Web::Http::Headers;

std::string GetAppPackageDetails()
{
  Package package{ Package::Current() };
  PackageId packageId{ package.Id() };
  PackageVersion version{ packageId.Version() };
  return fmt::format("winfetch: {}.{}.{}.{}", version.Major, version.Minor, version.Revision, version.Build);
}


IAsyncAction HandleURLFetch(Uri uri)
{
  HttpBaseProtocolFilter filter;
  filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::NoCache);
  filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::NoCache);
  filter.CookieUsageBehavior(HttpCookieUsageBehavior::NoCookies);
  filter.MaxVersion(HttpVersion::Http20);

  HttpClient client(filter);
  auto requestAsync = client.GetAsync(uri);

  requestAsync.Progress([](auto const& status, HttpProgress const& progress) {

    if (progress.Stage == HttpProgressStage::ReceivingContent) {
      double totalBytes = progress.TotalBytesToReceive ? unbox_value<int64_t>(progress.TotalBytesToReceive) : -1;
      double percentage = (double)progress.BytesReceived / (double)totalBytes;
      std::clog << percentage << "\n";
    }
    
  });

  auto result = co_await requestAsync;

  co_return;
}

int main(int argc, char* argv[])
{
  init_apartment();

  auto parser = argumentum::argument_parser{};
  parser.config().program("winfetch.exe").description(fmt::format("{}", GetAppPackageDetails()));

  std::optional<std::string> url;
  parser.add_argument(url, "-s").maxargs(1);

  if (!parser.parse_args(argc, argv, 1))
    return 1;

  try {
    auto fetchUri = Uri(to_hstring(*url));
    HandleURLFetch(fetchUri).get();
  }
  catch (hresult_error const& e) {
    std::cerr << fmt::format("Exception: [{:#08x}] {}\n", e.code(), to_string(e.message()));
  }

  uninit_apartment();
  return 0;
}
