#pragma once

#include "augmented_fstream.h"
#include <fstream>

#include <cstdint>

#include <string>
#include <string_view>

#include <vector>
#include <unordered_map>

const static uint8_t W3D_MAX_STRING_LENGTH = 0x10;
const static uint8_t MAX_SHORT_STRING_LENGTH = 0xff;

enum class ChunkType
{
    W3D_CHUNK_MESH = 0x0,
        W3D_CHUNK_VERTICES = 0x2,
        W3D_CHUNK_VERTEX_NORMALS = 0x3,
        W3D_CHUNK_MESH_USER_TEXT = 0xC,
        W3D_CHUNK_VERTEX_INFLUENCES = 0xE,
        W3D_CHUNK_MESH_HEADER3 = 0x1F,
        W3D_CHUNK_TRIANGLES = 0x20,
        W3D_CHUNK_VERTEX_SHADE_INDICES = 0x22,

        W3D_CHUNK_PRELIT_UNLIT,	// optional unlit material chunk wrapper
		W3D_CHUNK_PRELIT_VERTEX,	// optional vertex-lit material chunk wrapper
		W3D_CHUNK_PRELIT_LIGHTMAP_MULTI_PASS,	// optional lightmapped multi-pass material chunk wrapper
		W3D_CHUNK_PRELIT_LIGHTMAP_MULTI_TEXTURE,	// optional lightmapped multi-texture material chunk wrapper

        W3D_CHUNK_MATERIAL_INFO = 0x28,

        W3D_CHUNK_SHADERS,

        W3D_CHUNK_VERTEX_MATERIALS = 0x2A,
            W3D_CHUNK_VERTEX_MATERIAL,
                W3D_CHUNK_VERTEX_MATERIAL_NAME,
                W3D_CHUNK_VERTEX_MATERIAL_INFO,
                W3D_CHUNK_VERTEX_MAPPER_ARGS0,
                W3D_CHUNK_VERTEX_MAPPER_ARGS1,
        
        W3D_CHUNK_TEXTURES = 0x30,
            W3D_CHUNK_TEXTURE,
                W3D_CHUNK_TEXTURE_NAME,
                W3D_CHUNK_TEXTURE_INFO,
    
        W3D_CHUNK_MATERIAL_PASS = 0x38,
            W3D_CHUNK_VERTEX_MATERIAL_IDS,
            W3D_CHUNK_SHADER_IDS,
            W3D_CHUNK_DCG,
            W3D_CHUNK_DIG,
            W3D_CHUNK_SCG = 0x3E,
        
        W3D_CHUNK_SHADER_MATERIAL_ID = 0x3F,
        W3D_CHUNK_TEXTURE_STAGE = 0x48,
        W3D_CHUNK_TEXTURE_IDS = 0x49,
        W3D_CHUNK_STAGE_TEXCOORDS = 0x4A,
        W3D_CHUNK_PER_FACE_TEXCOORD_IDS = 0x4B,

        W3D_CHUNK_SHADER_MATERIALS = 0x50,
            W3D_CHUNK_SHADER_MATERIAL,
                W3D_CHUNK_SHADER_MATERIAL_HEADER,
                W3D_CHUNK_SHADER_MATERIAL_PROPERTY,
        
        W3D_CHUNK_TANGENTS = 0x60,
        W3D_CHUNK_BITANGENTS = 0x61,

        W3D_CHUNK_PS2_SHADERS = 0x80,

        W3D_CHUNK_AABTREE = 0x90,
            W3D_CHUNK_AABTREE_HEADER,
            W3D_CHUNK_AABTREE_POLYINDICES,
            W3D_CHUNK_AABTREE_NODES,

    W3D_CHUNK_HIERARCHY = 0x100,
        W3D_CHUNK_HIERARCHY_HEADER = 0x101,
        W3D_CHUNK_PIVOTS = 0x102,
        W3D_CHUNK_PIVOT_FIXUPS = 0x103,
    
    W3D_CHUNK_ANIMATION = 0x200,
        W3D_CHUNK_ANIMATION_HEADER,
        W3D_CHUNK_ANIMATION_CHANNEL,
        W3D_CHUNK_BIT_CHANNEL,
    
    W3D_CHUNK_COMPRESSED_ANIMATION = 0x280,
        W3D_CHUNK_COMPRESSED_ANIMATION_HEADER,
        W3D_CHUNK_COMPRESSED_ANIMATION_CHANNEL,
        W3D_CHUNK_COMPRESSED_BIT_CHANNEL,
        W3D_CHUNK_COMPRESSED_ANIMATION_MOTION_CHANNEL,
    
    W3D_CHUNK_MORPH_ANIMATION = 0x2C0,	                // hierarchy morphing animation data (morphs between poses, for facial animation)
		W3D_CHUNK_MORPHANIM_HEADER,						// W3dMorphAnimHeaderStruct describes playback rate, number of frames, and type of compression
		W3D_CHUNK_MORPHANIM_CHANNEL,					// wrapper for a channel
			W3D_CHUNK_MORPHANIM_POSENAME,				// name of the other anim which contains the poses for this morph channel
			W3D_CHUNK_MORPHANIM_KEYDATA,				// morph key data for this channel
		W3D_CHUNK_MORPHANIM_PIVOTCHANNELDATA,           // uin32 per pivot in the htree, indicating which channel controls the pivot

	W3D_CHUNK_HMODEL = 0x300,	            // blueprint for a hierarchy model
		W3D_CHUNK_HMODEL_HEADER,			// Header for the hierarchy model
		W3D_CHUNK_NODE,						// render objects connected to the hierarchy
		W3D_CHUNK_COLLISION_NODE,			// collision meshes connected to the hierarchy
		W3D_CHUNK_SKIN_NODE,				// skins connected to the hierarchy
		OBSOLETE_W3D_CHUNK_HMODEL_AUX_DATA,	// extension of the hierarchy model header
		OBSOLETE_W3D_CHUNK_SHADOW_NODE,		// shadow object connected to the hierarchy

	W3D_CHUNK_LODMODEL = 0x400,		        // blueprint for an LOD model. This is simply a
		W3D_CHUNK_LODMODEL_HEADER,	        // collection of 'n' render objects, ordered in terms
		W3D_CHUNK_LOD,				        // of their expected rendering costs. (highest is first)

	W3D_CHUNK_COLLECTION = 0x420,		    // collection of render object names
		W3D_CHUNK_COLLECTION_HEADER,		// general info regarding the collection
		W3D_CHUNK_COLLECTION_OBJ_NAME,		// contains a string which is the name of a render object
		W3D_CHUNK_PLACEHOLDER,				// contains information about a 'dummy' object that will be instanced later
		W3D_CHUNK_TRANSFORM_NODE,			// contains the filename of another w3d file that should be transformed by this node

	W3D_CHUNK_POINTS = 0x440,		        // array of W3dVectorStruct's. May appear in meshes, hmodels, lodmodels, or collections.

	W3D_CHUNK_LIGHT = 0x460,		        // description of a light
		W3D_CHUNK_LIGHT_INFO,				// generic light parameters
		W3D_CHUNK_SPOT_LIGHT_INFO,			// extra spot light parameters
		W3D_CHUNK_NEAR_ATTENUATION,			// optional near attenuation parameters
		W3D_CHUNK_FAR_ATTENUATION,			// optional far attenuation parameters
    
    W3D_CHUNK_EMITTER = 0x500,
        W3D_CHUNK_EMITTER_HEADER,
        W3D_CHUNK_EMITTER_USER_DATA,
        W3D_CHUNK_EMITTER_INFO,
        W3D_CHUNK_EMITTER_INFOV2,
        W3D_CHUNK_EMITTER_PROPS,
        W3D_CHUNK_EMITTER_LINE_PROPERTIES,
        W3D_CHUNK_EMITTER_ROTATION_KEYFRAMES,
        W3D_CHUNK_EMITTER_FRAME_KEYFRAMES,
        W3D_CHUNK_EMITTER_BLUR_TIME_KEYFRAMES,

    W3D_CHUNK_AGGREGATE	= 0x600,		    // description of an aggregate object
		W3D_CHUNK_AGGREGATE_HEADER,		    // general information such as name and version
			W3D_CHUNK_AGGREGATE_INFO,	    // references to 'contained' models
		W3D_CHUNK_TEXTURE_REPLACER_INFO,    // information about which meshes need textures replaced
		W3D_CHUNK_AGGREGATE_CLASS_INFO,		// information about the original class that created this aggregate
    
    W3D_CHUNK_HLOD = 0x700,
        W3D_CHUNK_HLOD_HEADER,
        W3D_CHUNK_HLOD_LOD_ARRAY,
            W3D_CHUNK_HLOD_SUB_OBJECT_ARRAY_HEADER,
            W3D_CHUNK_HLOD_SUB_OBJECT,
        W3D_CHUNK_HLOD_AGGREGATE_ARRAY,
        W3D_CHUNK_HLOD_PROXY_ARRAY,
    
    W3D_CHUNK_BOX = 0x740,
    W3D_CHUNK_SPHERE,
    W3D_CHUNK_RING,
    
    W3D_CHUNK_NULL_OBJECT = 0x750,

	W3D_CHUNK_LIGHTSCAPE = 0x800,
		W3D_CHUNK_LIGHTSCAPE_LIGHT,     // definition of a light created with Lightscape.
			W3D_CHUNK_LIGHT_TRANSFORM,	// position and orientation (defined as right-handed 4x3 matrix transform W3dLightTransformStruct).

	W3D_CHUNK_DAZZLE = 0x900,		    // wrapper for a glare object. Creates halos and flare lines seen around a bright light source
		W3D_CHUNK_DAZZLE_NAME,			// null-terminated string, name of the dazzle (typical w3d object naming: "container.object")
		W3D_CHUNK_DAZZLE_TYPENAME,		// null-terminated string, type of dazzle (from dazzle.ini)

	W3D_CHUNK_SOUNDROBJ	= 0xA00,
		W3D_CHUNK_SOUNDROBJ_HEADER,
		W3D_CHUNK_SOUNDROBJ_DEFINITION,
    
    W3D_CHUNK_VERTICES_2 = 0xC00,
    W3D_CHUNK_VERTEX_NORMALS_2,

    W3D_TEXTURE_FILE = 0xF00
};

struct Asset
{
    struct Chunk
    {
        friend Asset;

        const static std::unordered_map<uint32_t, ChunkType> PRIMARY_CHUNKS;
        const static std::unordered_map<ChunkType, uint32_t> PRIMARY_CHUNKS_INV;
        
        ChunkType type;
        std::string name{};
        uint32_t offset = 0;
        uint32_t size = 0;
        
        // Chunk's inputs (bare strings with names)
        std::vector<std::string> inputs{};

    private:
        std::vector<bool> validities{};

    private:
        void ReadInfoDat(augmented::ifstream&);
        void ReadInfoW3D(augmented::ifstream&);

        void Skip(std::ifstream&);
        
        template <typename Fn>
        void ReadHeader(std::ifstream& ifs, 
            ChunkType exp_header_type, 
            uint32_t exp_header_size, 
            uint32_t exp_version, 
            Fn&& fn, 
            uint8_t name_offset = 0);
        
        void ReadHeaderName(std::ifstream& ifs, 
            ChunkType exp_header_type, 
            uint32_t exp_header_size, 
            uint32_t exp_version,   
            uint8_t name_offset = 0);
        
        template <typename Fn>
        void ReadFromSubChunks(std::ifstream&, 
            uint32_t read_to, 
            ChunkType trg_type, 
            Fn&& fn);
        
        void ReadMesh(std::ifstream&);
        void ReadMeshTextures(std::ifstream&);

        void ReadHierarchy(std::ifstream&);

        void ReadAnimation(std::ifstream&);
        void ReadCompressedAnimation(std::ifstream&);

        void ReadEmitter(std::ifstream&);

        void ReadAggregate(std::ifstream&);

        void ReadHLoD(std::ifstream&);
        void ReadHLoDSubObjects(std::ifstream&);

        void ReadBox(std::ifstream&);
        
    public:
        Chunk() = default;
        Chunk(const Chunk&) = default;
        Chunk(Chunk&&) noexcept;
        Chunk(augmented::ifstream&);
        ~Chunk() = default;

        void swap(Chunk&);

        void SetUpValidities();
        bool IsValidInput(size_t i) const;
        void ValidateInput(size_t i);
        void InvalidateInput(size_t i);

        augmented::ifstream& operator>>(augmented::ifstream&);
        friend augmented::ifstream& operator>>(augmented::ifstream&, Chunk& chunk);

        std::ofstream& operator<<(std::ofstream&) const;
        friend std::ofstream& operator<<(std::ofstream&, const Chunk& chunk);
    };

    class invalid_file_format : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    std::string name{};
    size_t size = 0;
    uint64_t time = 0;
    std::vector<Chunk> chunks{}; // Primary chunks making up an asset

private:
    void GetFileAttrs(const std::string& path);
    void ReadInfoDat(augmented::ifstream&);
    void ReadInfoTex(augmented::ifstream&);
    void ReadInfoW3D(augmented::ifstream&);

public:
    Asset(Asset&&) noexcept;
    Asset(augmented::ifstream&);

    void swap(Asset&);

    bool operator==(const Asset& other) const;
    bool operator<(const Asset& other) const;
    bool operator>(const Asset& other) const;

    augmented::ifstream& operator>>(augmented::ifstream& ifs);
    friend augmented::ifstream& operator>>(augmented::ifstream& ifs, Asset& asset);

    std::ofstream& operator<<(std::ofstream& ofs) const;
    friend std::ofstream& operator<<(std::ofstream& ofs, const Asset& asset);
};