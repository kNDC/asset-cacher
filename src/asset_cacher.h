#pragma once
#include "w3d.h"
#include "binary_io.h"
#include "container_utils.h"
#include "console_progress_bar.h"

#include <cwctype>

#include <iostream>
#include <format>

#include <vector>
#include <unordered_map>

#include <filesystem>

class AssetCacher
{
private:
    template <typename SV>
    struct CN_Hasher
    {
        /* A case-neutral implementation of 
        the DJB2 algorithm.  See the following 
        for further discussion and other algorithms: 
        http://www.cse.yorku.ca/~oz/hash.html */
        size_t operator()(SV sv) const
        {
            size_t hash = 5381;

            for (size_t i = 0; i < sv.size(); ++i)
            {
                char c = std::towlower(sv[i]);
                hash = hash * 33 ^ c;
            }
            
            return hash;
        }
    };

    // Case-neutral string comparator
    template <typename SV>
    struct CN_Equals
    {
    private:
        bool CompareEqualLength(SV sv1, SV sv2) const
        {
            for (size_t i = 0; i < sv1.size(); ++i)
            {
                if (sv1[i] == sv2[i]) continue;
                if (std::towlower(sv1[i]) == 
                    std::towlower(sv2[i])) continue;
                return false;
            }
            
            return true;
        }

    public:
        bool operator()(SV sv1, SV sv2) const
        {
            return sv1.size() == sv2.size() && 
                CompareEqualLength(sv1, sv2);
        }
    };
    
    using File = std::filesystem::directory_entry;
    using AssetsDict = std::unordered_map<std::string_view, size_t, 
        CN_Hasher<std::string_view>, CN_Equals<std::string_view>>;
    using ChunksDict = std::unordered_map<size_t, 
        std::unordered_map<std::string_view, size_t, 
            CN_Hasher<std::string_view>, 
            CN_Equals<std::string_view>>>;
    using ChunkNames = std::unordered_map<std::string_view, 
        std::pair<size_t, size_t>, CN_Hasher<std::string_view>, 
        CN_Equals<std::string_view>>;
    
    // Sorted vector of acceptable formats
    const static std::vector<std::string_view> formats;

    const std::string root_path_;
    
    size_t n_assets_ = 0; // max total of assets
    size_t n_dat_assets_ = 0; // total of existing assets

    // Total of chunk inputs/dependencies
    size_t n_inputs_ = 0;

    // Asset collections and dictionaries
    std::vector<Asset> assets_; // all assets
    AssetsDict assets_dict_; // index of all assets
    ChunksDict chunks_dict_; // index of all chunks sectioned by assets
    ChunkNames chunk_names_dict_; // index of all chunks' names
        
private:
    bool IsValidFile(const File& file);

    void CountNewFiles();
    uint64_t ReadDatHeader(std::ifstream& ifs);

    template <typename Invalidator, typename Decrementor>
    void ProcessInputs(Invalidator&& inv, 
    Decrementor&& dec);

    void ImportExistAssetData(augmented::ifstream& ifs);
    void ImportExistInputData(augmented::ifstream& ifs);

    void AddAsset(Asset&& asset, size_t i);
    void AddAsset(Asset&& asset);
    
    void WriteDatHeader(std::ofstream& ofs) const;
    void ExportAssetData(std::ofstream& ofs) const;
    void ExportInputData(std::ofstream& ofs) const;

public:
    template <typename S>
    AssetCacher(S&& s);
    AssetCacher() : 
        AssetCacher::AssetCacher(".\\")
    {}

    ~AssetCacher() = default;

    void ImportExistData();
    void ImportNewData();
    void ValidateInputs();
    void FilterInputs();
    void ExportData() const;
};

template <typename Invalidator, typename Decrementor>
void AssetCacher::ProcessInputs(Invalidator &&inv,
                                Decrementor &&dec)
{
    std::cout << "**Validating inputs\n";

    std::ofstream warnings;
    ProgressBar bar(n_inputs_);
    size_t n_missing_inputs = 0;

    for (Asset& asset : assets_)
    {
        for (Asset::Chunk& chunk : asset.chunks)
        {
            size_t n_valid_inputs = chunk.inputs.size();

            for (size_t i = 0; i < chunk.inputs.size(); ++i)
            {
                if (chunk_names_dict_.find(chunk.inputs[i]) == 
                    chunk_names_dict_.end())
                {
                    if (warnings.is_open()) warnings << '\n';
                    else warnings.open(root_path_ + "warnings.log");
                    
                    inv(chunk, i);
                    n_valid_inputs = dec(n_valid_inputs);

                    warnings << std::format("Chunk {0} in asset {1} has "
                        "an unresolved dependency: {2};", 
                        chunk.name, asset.name, chunk.inputs[i]);
                    ++n_missing_inputs;
                }
            }
            
            if (chunk.inputs.size() && 
                !n_valid_inputs) --n_inputs_;
            ++bar;
        }
    }
    std::cout << '\n';

    std::cout << n_missing_inputs << " record(s) with missing inputs.\n";
}

template <typename S>
AssetCacher::AssetCacher(S&& s) : 
    root_path_{std::forward<S>(s)}
{
    std::error_code ec;
    std::filesystem::remove(root_path_ + "warnings.log", ec);
    CountNewFiles();
}