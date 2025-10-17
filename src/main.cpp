#include "asset_cacher.h"
#include "settings.h"

static Settings<char> settings;

int main()
{
    settings.Read();
    settings.Set();
    settings.Save();
	
    AssetCacher asset_cacher;
    
    if (settings.incremental)
    {
        asset_cacher.ImportExistData();
    }
    
    asset_cacher.ImportNewData();

    switch (settings.input_policy)
    {
    case Settings<char>::InputPolicy::Informative:
        asset_cacher.ValidateInputs();
        break;
    
    case Settings<char>::InputPolicy::Pedantic:
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