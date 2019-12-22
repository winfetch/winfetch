#include "pch.h"

#include <filesystem>

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

  CLI::App app(GetAppPackageDetails());

  // XXX: By default, the app name will have the AppData folder, something the normal user
  // won't care about. Let's just make sure we're showing the name they would type in.
  app.name("winfetch.exe");
  app.prefix_command();

  std::string url;
  CLI::Option* opt = app.add_option("url", url, "URL");
  opt->required(true);

  
  CLI11_PARSE(app, argc, argv);

  try {
    auto fetchUri = Uri(to_hstring(url));
    HandleURLFetch(fetchUri).get();
  }
  catch (hresult_error const& e) {
    std::cerr << fmt::format("Exception: [{:#08x}] {}\n", e.code(), to_string(e.message()));
  }

  uninit_apartment();
  return 0;
}
