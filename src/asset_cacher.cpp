#include "asset_cacher.h"
#include <iostream>
#include <format>

const std::vector<std::string_view> AssetCacher::formats =
{
    ".dds",
    ".jfif", ".jpeg", ".jpg",
    ".png",
    ".tga",
    ".w3d"
};

bool AssetCacher::IsValidFile(const File& file)
{
    using namespace std::string_view_literals;

    if (!file.is_regular_file()) return false;

    std::string ext = file.path().extension().string();

    std::vector<std::string_view>::const_iterator pos;
    pos = BinarySearch(formats.begin(), formats.end(), ext);
    return (pos != formats.end());
}

uint64_t AssetCacher::ReadDatHeader(std::ifstream& ifs)
{
    if (!ifs.is_open()) return 0;

    char signature[4];
    *(uint32_t*)signature = ReadPrimitive<uint32_t>(ifs);
    if (signature[0] != 'A' ||
        signature[1] != 'L' ||
        signature[2] != 'A' ||
        signature[3] != 'E') return 0;

    char version[4];
    *(uint32_t*)version = ReadPrimitive<uint32_t>(ifs);
    if (version[0] != '\2' ||
        version[1] != '\1' ||
        version[2] != '\0' ||
        version[3] != '\0') return 0;

    return ReadPrimitive<uint64_t>(ifs);
}

void AssetCacher::CountNewFiles()
{
    for (const std::filesystem::directory_entry& file :
        std::filesystem::recursive_directory_iterator(root_path_))
    {
        n_assets_ += (IsValidFile(file)) ? 1 : 0;
    }
}

void AssetCacher::ImportExistAssetData(augmented::ifstream& ifs)
{
    std::cout << "**Reading assets from the source .dat file\n";
    ProgressBar bar(n_dat_assets_);

    for (size_t i = 0; i < n_dat_assets_; ++i)
    {
        assets_.emplace_back(ifs);
        assets_dict_[assets_.back().name] = i;

        size_t j = 0;
        for (const Asset::Chunk& chunk : assets_.back().chunks)
        {
            chunks_dict_[i][chunk.name] = j++;
            chunk_names_dict_[chunk.name] = { i, j };
        }

        ++bar;
    }
    std::cout << '\n';
    std::cout << n_dat_assets_ << " asset(s) imported.\n";
}

void AssetCacher::ImportExistInputData(augmented::ifstream& ifs)
{
    using namespace std::string_view_literals;

    std::cout << "**Reading input records from the source .dat file\n";
    ProgressBar bar(n_inputs_);

    for (size_t i = 0; i < n_inputs_; ++i)
    {
        std::string name = ReadShortString(ifs.FS());
        size_t asset_index = assets_dict_.at(name);

        name = ReadShortString(ifs.FS());
        size_t chunk_index = chunks_dict_.at(asset_index).at(name);
        
        Asset::Chunk& chunk = assets_[asset_index].chunks[chunk_index];
        chunk.inputs.reserve(ReadPrimitive<uint16_t>(ifs.FS()));
        chunk.SetUpValidities();

        for (size_t j = 0; j < chunk.inputs.capacity(); ++j)
        {
            chunk.inputs.emplace_back(ReadShortString(ifs.FS()));
        }
        
        ++bar;
    }
    std::cout << '\n';
    std::cout << n_inputs_ << " input record(s) imported.\n";
}

void AssetCacher::AddAsset(Asset&& asset, size_t i)
{
    // Removing old records in dictionaries
    if (i < assets_.size() && 
        asset.chunks.size())
    {
        assets_dict_.erase(assets_[i].name);
        chunks_dict_.at(i).clear();
    
        for (const Asset::Chunk& chunk : 
            assets_[i].chunks)
        {
            chunk_names_dict_.erase(chunk.name);
            n_inputs_ -= (chunk.inputs.size()) ? 1 : 0;
        }
    }

    // Updating the asset and the asset dictionary
    if (i < assets_.size()) assets_[i].swap(asset);
    else assets_.emplace_back(std::move(asset));
    assets_dict_[assets_[i].name] = i;
    
    // Adding new records to chunk dictionaries
    size_t j = 0;
    for (const Asset::Chunk& chunk : assets_[i].chunks)
    {
        chunks_dict_[i][chunk.name] = j++;
        chunk_names_dict_[chunk.name] = { i, j };
        n_inputs_ += (chunk.inputs.size()) ? 1 : 0;
    }
}

void AssetCacher::AddAsset(Asset&& asset)
{
    AddAsset(std::move(asset), assets_.size());
}

void AssetCacher::WriteDatHeader(std::ofstream& ofs) const
{
    ofs.write("ALAE", 4);
    ofs.write("\2\1\0\0", 4);

    WritePrimitive(ofs, (uint32_t)n_assets_);
    WritePrimitive(ofs, (uint32_t)n_inputs_);
}

void AssetCacher::ExportAssetData(std::ofstream& ofs) const
{
    std::cout << "**Exporting asset data\n";
    ProgressBar bar(n_assets_);

    for (const Asset& asset : assets_)
    {
        ofs << asset;
        ++bar;
    }
    std::cout << '\n';
    std::cout << n_assets_ << " asset(s) exported.\n";
}

void AssetCacher::ExportInputData(std::ofstream& ofs) const
{
    std::cout << "**Exporting input records\n";
    ProgressBar bar(n_inputs_);

    size_t i = 0;
    for (const Asset& asset : assets_)
    {
        for (const Asset::Chunk& chunk : asset.chunks)
        {
            size_t n_valid_inputs = 0;
            for (size_t i = 0; i < chunk.inputs.size(); ++i)
            {
                n_valid_inputs += chunk.IsValidInput(i) ? 1 : 0;
            }

            if (!n_valid_inputs) continue;
            
            WriteShortString(ofs, asset.name);
            WriteShortString(ofs, chunk.name);
            WritePrimitive<uint16_t>(ofs, n_valid_inputs);

            for (size_t i = 0; i < chunk.inputs.size(); ++i)
            {
                if (!chunk.IsValidInput(i)) continue;
                WriteShortString(ofs, chunk.inputs[i]);
            }

            ++bar;
        }
    }
    std::cout << '\n';
    std::cout << n_inputs_ << " input record(s) exported.\n";
}

void AssetCacher::ImportExistData()
{
    augmented::ifstream ifs(root_path_ + "asset.dat",
        std::ios::binary);
    
    {
        uint64_t totals = ReadDatHeader(ifs.FS());
        n_dat_assets_ = ((uint32_t*)&totals)[0];
        n_assets_ += n_dat_assets_;
        n_inputs_ += ((uint32_t*)&totals)[1];
    }

    assets_.reserve(n_assets_);

    if (n_dat_assets_ && 
        n_assets_ == (uint32_t)n_assets_)
    {
        ImportExistAssetData(ifs);
    }

    if (n_inputs_ &&
        n_inputs_ == (uint32_t)n_inputs_)
    {
        ImportExistInputData(ifs);
    }
}

void AssetCacher::ImportNewData()
{
    /* If the cache is made incrementally, 
    assets_ already has the necessary capacity
    reserved in ImportNewData => nothing is done 
    here in this case.  Relocation is made only 
    when the vector of assets is still empty. */
    assets_.reserve(n_assets_);

    std::cout << "**Reading asset files\n";

    /* Non-zero size of assets_ can come only from 
    assets picked from the source .dat file with 
    incremental generation. */
    ProgressBar bar(n_assets_ - assets_.size());
    size_t n_new_assets = 0;
    size_t n_upd_assets = 0;

    size_t i = 0;
    for (const std::filesystem::directory_entry& file :
        std::filesystem::recursive_directory_iterator(root_path_))
    {
        if (!file.is_regular_file()) continue;

        augmented::ifstream ifs;
        {
            std::string path = file.path().string();
            for (char& c : path) c = std::tolower(c);

            ifs.Open(std::move(path), std::ios::binary,
                formats, true);
        }
        if (!ifs.FS().is_open()) continue;

        try
        {
            Asset asset(ifs);
            AssetsDict::iterator pos =
                assets_dict_.find(asset.name);
            
            // The case of a new asset
            if (pos == assets_dict_.end())
            {
                AddAsset(std::move(asset));
                ++n_new_assets;
            }
            /* The case of an overlapping asset.
            Is it newer than the one already included? */
            else if (asset > assets_[pos->second])
            {
                size_t i = pos->second; // index of the asset to be replaced
                AddAsset(std::move(asset), i);
                ++n_upd_assets;
            }
        }
        catch (const Asset::invalid_file_format& e)
        {
            std::cerr << e.what() << '\n';
        }

        ++bar;
    }
    std::cout << '\n';
    std::cout << n_new_assets << " asset(s) added.\n";
    std::cout << n_upd_assets << " asset(s) updated.\n";

    n_assets_ = assets_.size();
}

void AssetCacher::ValidateInputs()
{
    ProcessInputs([](Asset::Chunk&, size_t i) {}, 
        [](size_t n_valid_inputs) { return n_valid_inputs; });
}

void AssetCacher::FilterInputs()
{
    ProcessInputs([](Asset::Chunk& chunk, size_t i)
        {
            chunk.InvalidateInput(i);
        }, 
        [](size_t n_valid_inputs) 
        { 
            return --n_valid_inputs;
        });
}

void AssetCacher::ExportData() const
{
    using namespace std::filesystem;

    // Backing up the existing .dat file (if there is such)
    {
        std::error_code ec;
        copy_file(root_path_ + "asset.dat", 
            root_path_ + "asset.dat.bak", 
            std::filesystem::copy_options::overwrite_existing, 
            ec);
    }
    
    path output_path(root_path_ + "asset.dat");
    std::ofstream ofs(output_path, std::ios::binary); // <-- File picker window here in the future...
    if (ofs.bad()) throw std::runtime_error("Unable to write the output file!");

    WriteDatHeader(ofs);

    try
    {
        ExportAssetData(ofs);
    }
    catch(const std::exception& e)
    {
        ofs.close();

        std::error_code ec;
        remove(output_path, ec);

        throw std::runtime_error(e.what());
    }
    
    try
    {
        ExportInputData(ofs);
    }
    catch(const std::exception& e)
    {
        ofs.close();

        std::error_code ec;
        remove(output_path, ec);

        throw std::runtime_error(e.what());
    }
}