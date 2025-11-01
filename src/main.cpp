#include "asset_cacher.h"
#include "config.h"

static Config<char> config;

int main()
{
    config.Read();
    config.Set();
    config.Save();
	
    AssetCacher asset_cacher;
    
    if (config.incremental)
    {
        asset_cacher.ImportExistData();
    }
    
    asset_cacher.ImportNewData();

    switch (config.input_policy)
    {
    case Config<char>::InputPolicy::Informative:
        asset_cacher.ValidateInputs();
        break;
    
    case Config<char>::InputPolicy::Pedantic:
        asset_cacher.FilterInputs();
        break;
    
    default:
        break;
    }
    
    try
    {
        asset_cacher.ExportData();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    std::cout << "Done!\n";
    std::cout << "Press any key to exit...";
    _getch();
}