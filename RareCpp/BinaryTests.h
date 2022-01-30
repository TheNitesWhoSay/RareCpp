#pragma once
#include "../RareCppLib/Binary.h"
#include <fstream>
#include <limits>
#include <iostream>
#include <sstream>
#include <deque>
#include <memory>
#include <string>
using Reflect::class_t;

class Exception : public std::exception
{
public:
    Exception(const char* what) : message(what) {}

    virtual const char* what() const noexcept { return message.c_str(); }

private:
    std::string message;
};

enum class SectionName : uint32_t
{
    TYPE = 1162893652, VER = 542262614, IVER = 1380275785, IVE2 = 843404873,
    VCOD = 1146045270, IOWN = 1314344777, OWNR = 1380865871, ERA = 541151813,
    DIM = 541935940, SIDE = 1162103123, MTXM = 1297634381, PUNI = 1229870416,
    UPGR = 1380405333, PTEC = 1128617040, UNIT = 1414090325, ISOM = 1297044297,
    TILE = 1162627412, DD2 = 540165188, THG2 = 843532372, MASK = 1263747405,
    STR = 542266451, UPRP = 1347571797, UPUS = 1398100053, MRGN = 1313296973,
    TRIG = 1195987540, MBRF = 1179796045, SPRP = 1347571795, FORC = 1129467718,
    WAV = 542523735, UNIS = 1397313109, UPGS = 1397182549, TECS = 1396917588,
    SWNM = 1296979795, COLR = 1380732739, PUPx = 2018530640, PTEx = 2017809488,
    UNIx = 2018070101, UPGx = 2017939541, TECx = 2017674580, STRx = 0xFEEBDAED,

    OSTR = 1381258063, KSTR = 1381258059, KTRG = 1196577867, KTGP = 1346851915,

    UNKNOWN = 0,
    //UNKNOWN = 0xFFFFFFFF
};

enum class LoadBehavior
{
    Standard,
    Override,
    Append
};

struct RawSection {
    RawSection() {}
    RawSection(SectionName sectionName) : sectionName(sectionName) {}

    SectionName sectionName = SectionName::UNKNOWN;
    std::unique_ptr<std::vector<uint8_t>> rawData = nullptr;
    char* getBuffer(size_t new_cap)
    {
        if ( rawData == nullptr )
            rawData = std::make_unique<std::vector<uint8_t>>();

        rawData->resize(new_cap);
        return reinterpret_cast<char*>(rawData->data());
    }

    NOTE(customSize, Binary::Ignore)
    int32_t customSize = 0; /** In the serialized header section size will be set to custom size only in the following cases...
                                1.) customSize < 0: a "jump section" is formed
                                2.) customSize > 0 && customSize < dataSize: only customSize bytes are written, other data is not serialized
                                3.) customSize > 0 && customSize > dataSize && isLastSection: an "oversized trailing section" is formed */

    bool isParsed() const { return rawData == nullptr; }
    void clearData() { this->rawData = nullptr; this->customSize = 0; }

    REFLECT(RawSection, sectionName, rawData, customSize)
};

enum class Version : uint32_t
{
    BroodWar_1_16_1
};

enum class IVersion : uint32_t
{

};


enum class I2Version : uint32_t
{

};

enum class Type : uint32_t
{
};

enum_t(ValidationOpCodes, uint8_t, { // u8
    XOR_Shifted_Sections = 0, // XOR the current hash with total values of OWNR, SIDE*256, FORC*65536
    Add_Shifted_Sections = 1, // Adds to the current hash with the total values of OWNR, SIDE*256, FORC*65536
    Sub_Shifted_Sections = 2, // Subtracts from the current hash with the total values of OWNR, SIDE*256, FORC*65536
    XOR_Sections = 3, // XOR the current hash with the VCOD table at index OWNR, SIDE, FORC, and 0
    XOR_Sections_04 = 4, // Same as XOR_Sections
    XOR_Sections_05 = 5, // Same as XOR_Sections
    ORs_And_Shifts = 6, // Complex function consisting of ORs and shifts
    ORs_And_Reverse_Shifts = 7, // Same as ORS_AND_SHIFTS with shift direction inverted
    NOOP = 8 // No operation
});
    
constexpr uint32_t TotalValidationSeeds = 256;
constexpr uint32_t TotalValidationOpCodes = 16;

struct VCOD
{
    uint32_t seedValues[TotalValidationSeeds];
    ValidationOpCodes opCodes[TotalValidationOpCodes];

    REFLECT(VCOD, seedValues, opCodes)
};

struct Unit
{
    uint16_t unitType = 0;
    uint32_t hp = 0;

    REFLECT(Unit, unitType, hp)
};

using Tile = uint16_t;

// Binary: StarCraft representation only; JSON Default: Uses numbers; JSON Human: decodes numbers (e.g. 0 becomes "Terran Marine"), and inline's strings
NOTE(Map, Binary::FieldPrefixType<SectionName>, Binary::PrefixFieldSizes)
struct Map
{
    Version ver; // StarCraft version
    Type type; // Redundant versioning
    IVersion iver; // Redundant versioning
    I2Version ive2; // Redundant versioning
    VCOD vcod; // Validation
    std::vector<Unit> units;
    std::vector<Tile> tiles; // Final tiles (MTXM)
    std::vector<Tile> editorTiles; // Editor tiles (TILE)
    std::deque<std::unique_ptr<std::string>> strings; // Map strings (STR/STRx)
    std::deque<std::unique_ptr<std::string>> editorStrings; // Editor strings (KSTR)

    /// If non-empty, this field determines section inclusion and ordering as well as holding the data of any unparsed sections
    /// When serializing, if a section.isParsed(): use the map data field(s); if a section is unparsed: put the rawData from Section
    std::deque<RawSection> sections;
    std::vector<uint8_t> tailBytes; // If size > 0, this is the last 1 to 7 bytes in the chk (incomplete section header)

    NOTE(type, SectionName::TYPE)
    NOTE(ver, SectionName::VER)
    NOTE(iver, SectionName::IVER)
    NOTE(ive2, SectionName::IVE2)
    NOTE(vcod, SectionName::VCOD)
    NOTE(tiles, SectionName::MTXM, LoadBehavior::Override)
    NOTE(units, SectionName::UNIT, LoadBehavior::Append)
    NOTE(editorTiles, SectionName::TILE)
    NOTE(strings, SectionName::STR, LoadBehavior::Override) // ver, the volume of string data, or the "sections" setting, may change SectionName to STRx
    NOTE(editorStrings, SectionName::KSTR)

    REFLECT_NOTED(Map, type, ver, iver, ive2, vcod, tiles, units, editorTiles, strings, editorStrings)

    template <typename T>
    void serialize(std::ostream & os, SectionName sectionName, T & section) const
    {
        if ( sectionName == SectionName::KSTR )
            serializeKstr(os);
        else // Every known section except strings
        {
            std::stringstream serializedSection(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
            auto start = serializedSection.tellp();
            serializedSection << Binary::out(section);
            auto sectionSize = serializedSection.tellp() - start;
            os << Binary::out(sectionName) << Binary::out(int32_t(sectionSize));
            if ( sectionSize > 0 )
                os << serializedSection.rdbuf();
        }
    }

    void serialize(std::ostream & os) const // to binary
    {
        if ( sections.empty() ) // Use order of sections in reflected fields/staredit default ordering
        {
            class_t<Map>::ForEachField(*this, [&](auto & field, auto & value) {
                using Field = std::remove_reference_t<decltype(field)>;
                if constexpr ( Field::template HasAnnotation<SectionName> )
                {
                    std::cout << field.name << std::endl;
                    serialize(os, field.template getAnnotation<SectionName>(), value);
                }
            });
        }
        else // Use order determined by "sections"
        {
            auto sectionNamesToFieldIndexes = getSectionNamesToFieldIndexes();
            for ( const auto & section : sections )
            {
                auto sectionName = section.sectionName;
                auto foundFieldIndex = sectionNamesToFieldIndexes.find(sectionName);
                if ( section.isParsed() && foundFieldIndex != sectionNamesToFieldIndexes.end() ) // section is parsed & maps to a known field index
                {
                    class_t<Map>::FieldAt(*this, foundFieldIndex->second, [&](auto & field, auto & value) {
                        serialize(os, sectionName, value);
                    });
                }
                else // section is unparsed and/or unknown
                    os << Binary::out(section);
            }
        }
    }

    void deserialize(std::istream & is)
    {
        auto sectionNamesToFieldIndexes = getSectionNamesToFieldIndexes();
        sections.clear();
        while ( is.good() )
        {
            constexpr size_t headerSize = 8;
            uint8_t header[headerSize] {};
            is.read((char*)&header[0], std::streamsize(headerSize));
            std::streamsize headerBytesRead = is.good() ? headerSize : (is.eof() ? is.gcount() : 0);
            if ( headerBytesRead == 0 && is.eof() )
                break;
            else if ( headerBytesRead == 0 )
                throw Exception("Unexpected failure reading chk section header!");
            else if ( headerBytesRead == headerSize )
            {
                SectionName & sectionName = *reinterpret_cast<SectionName*>(&header[0]);
                uint32_t & sectionSize = *reinterpret_cast<uint32_t*>(&header[4]);

                char sectionNameStr[5] = {};
                memcpy(sectionNameStr, &sectionName, 4);
                auto found = sectionNamesToFieldIndexes.find(sectionName);
                // TODO: Validate section size & handle mismatches
                if ( found != sectionNamesToFieldIndexes.end() )
                {
                    class_t<Map>::FieldAt(*this, found->second, [&](auto & field, auto & sectionData) {
                        using SectionData = std::remove_reference_t<decltype(sectionData)>;
                        LoadBehavior loadBehavior = field.template getAnnotation<LoadBehavior>();
                        if ( loadBehavior == LoadBehavior::Override )
                        {
                            std::cout << "Override section: " << sectionNameStr << std::endl;
                            //is >> Binary::in(sectionData, size_t(sectionSize));

                            auto existing = std::find_if(sections.begin(), sections.end(),
                                [&](const RawSection & section){ return sectionName == section.sectionName; });
                            auto & rawSection = existing != sections.end() ? *existing : sections.emplace_back(sectionName);
                            is.read(rawSection.getBuffer(sectionSize), std::streamsize(sectionSize));
                            //std::streamsize bytesRead = is.good() ? sectionSize : (is.eof() ? is.gcount() : 0);
                            //if ( bytesRead != std::streamsize(sectionSize) )
                            //  ; // ??
                        }
                        else if ( ExtendedTypeSupport::is_stl_iterable<SectionData>::value )
                        {
                            std::cout << "Iterable section: " << sectionNameStr << std::endl;
                            is >> Binary::in(sectionData, size_t(sectionSize));
                        }
                        else
                        {
                            std::cout << "Regular section: " << sectionNameStr << std::endl;
                            is >> Binary::in(sectionData);
                        }
                    });
                }
                else // Unknown section
                {
                    std::cout << "Unknown section: " << sectionNameStr << std::endl;
                    RawSection & rawSection = sections.emplace_back(RawSection{sectionName});
                    is.read((char*)rawSection.getBuffer(sectionSize), std::streamsize(sectionSize));
                    //std::streamsize bytesRead = is.good() ? sectionSize : (is.eof() ? is.gcount() : 0);
                    //if ( bytesRead != std::streamsize(sectionSize) )
                    //  ; // ??
                }
            }
        }
        // Validate
        // Load MTXM, STR/STRx from rawSection
        auto mtxm = std::find_if(sections.begin(), sections.end(),
            [&](const RawSection & section){ return SectionName::MTXM == section.sectionName; });
        if ( mtxm != sections.end() && mtxm->rawData != nullptr )
        {
            auto & rawData = *(mtxm->rawData);
            if ( rawData.size() % 2 == 1 )
                rawData.push_back(uint8_t(0));

            uint8_t* dat = rawData.data();
            size_t numTiles = rawData.size() / 2;
            for ( size_t i=0; i<numTiles; ++i )
                this->tiles.push_back(*((Tile*)&dat[i*2]));

            mtxm->clearData();
        }
        // Validate
    }

    std::unordered_map<SectionName, size_t> getSectionNamesToFieldIndexes() const
    {
        std::unordered_map<SectionName, size_t> sectionNamesToFieldIndexes{};
        class_t<Map>::ForEachField(*this, [&](auto & field, auto & value) {
            using Field = std::remove_reference_t<decltype(field)>;
            if constexpr ( Field::template HasAnnotation<SectionName> )
            {
                auto sectionName = field.template getAnnotation<SectionName>();
                sectionNamesToFieldIndexes.insert(std::make_pair(sectionName, Field::Index));
            }
        });
        return sectionNamesToFieldIndexes;
    }
    void serializeStr(std::ostream & os, const RawSection* section = nullptr) const
    {
        os << Binary::out(SectionName::STR);
        os << Binary::out((int32_t)0);
    }
    void serializeStrX(std::ostream & os, const RawSection* section = nullptr) const { /* ... */ }
    void serializeKstr(std::ostream & os, const RawSection* section = nullptr) const
    {
        os << Binary::out(SectionName::KSTR);
        os << Binary::out((int32_t)0);
    }

    void deserializeStr(std::istream & is)
    {
    
    }

    void deserializeKstr(std::istream & is)
    {
    
    }
};

/**template <> struct Binary::Output::CustomizeType<Map>
{
    static bool As(OutStreamType & output, Context & context, const Map & map)
    {
        map.serialize(output);
        return true;
    }
};

template <> struct Binary::Output::Customize<Map, decltype(Map::strings), class_t<Map>::strings_::Field::Index>
{
    static bool As(OutStreamType & output, Context & context, const Map & object, const decltype(Map::strings) & strings)
    {
        return true;
    }
};*/

void binaryTests()
{
    Map map { Version::BroodWar_1_16_1, (Type)0, (IVersion)0, (I2Version)0,
        {{
            0x77CA1934, 0x7168DC99, 0xC3BF600A, 0xA775E7A7, 0xA67D291F, 0xBB3AB0D7, 0xED2431CC, 0x0B134C17, 0xB7A22065, 0x6B18BD91, 0xDD5DC38D, 0x37D57AE2,
            0xD46459F6, 0x0F129A63, 0x462E5C43, 0x2AF874E3, 0x06376A08, 0x3BD6F637, 0x1663940E, 0xEC5C6745, 0xB7F77BD7, 0x9ED4FC1A, 0x8C3FFA73, 0x0FE1C02E,
            0x070974D1, 0xD764E395, 0x74681675, 0xDA4FA799, 0x1F1820D5, 0xBEA0E6E7, 0x1FE3B6A6, 0x70EF0CCA, 0x311AD531, 0x3524B84D, 0x7DC7F8E3, 0xDE581AE1,
            0x432705F4, 0x07DBACBA, 0x0ABE69DC, 0x49EC8FA8, 0x3F1658D7, 0x8AC1DBE5, 0x05C0CF41, 0x721CCA9D, 0xA55FB1A2, 0x9B7023C4, 0x14E10484, 0xDA907B80,
            0x0669DBFA, 0x400FF3A3, 0xD4CEF3BE, 0xD7CBC9E3, 0x3401405A, 0xF81468F2, 0x1AC58E38, 0x4B3DD6FE, 0xFA050553, 0x8E451034, 0xFE6991DD, 0xF0EEE0AF,
            0xDD7E48F3, 0x75DCAD9F, 0xE5AC7A62, 0x67621B31, 0x4D36CD20, 0x742198E0, 0x717909FB, 0x7FCD6736, 0x3CD65F77, 0xC6A6A2A2, 0x6ACEE31A, 0x6CA9CD4E,
            0x3B9DBA86, 0xFD76F4B5, 0xBCF044F8, 0x296EE92E, 0x6B2F2523, 0x4427AB08, 0x99CC127A, 0x75F2DCED, 0x7E383CC5, 0xC51B1CF7, 0x65942DD1, 0xDD48C906,
            0xAC2D32BE, 0x8132C9B5, 0x34D84A66, 0xDF153F35, 0xB6EBEEB2, 0x964DF604, 0x9C944235, 0x61D38A62, 0x6F7BA852, 0xF4FC61DC, 0xFE2D146C, 0x0AA4EA99,
            0x13FED9E8, 0x594448D0, 0xE3F36680, 0x198DD934, 0xFE63D716, 0x3A7E1830, 0xB10F8D9B, 0x8CF5F012, 0xDB58780A, 0x8CB8633E, 0x8EF3AA3A, 0x2E1A8A37,
            0xEFF9315C, 0x7EE36DE3, 0x133EBD9B, 0xB9C044C6, 0x90DA3ABC, 0x74B0ADA4, 0x892757F8, 0x373FE647, 0x5A7942E4, 0xEE8D43DF, 0xE8490AB4, 0x1A88C33C,
            0x766B0188, 0xA3FDC38A, 0x564E7A16, 0xBACB7FA7, 0xEC1C5E02, 0x76C9B9B0, 0x39B1821E, 0xC557C93E, 0x4C382419, 0xB8542F5D, 0x8E575D6F, 0x520AA130,
            0x5E71186D, 0x59C30613, 0x623EDC1F, 0xEBB5DADC, 0xF995911B, 0xDAD591A7, 0x6BCE5333, 0x017000F5, 0xE8EED87F, 0xCEF10AC0, 0xD3B6EB63, 0xA5CCEF78,
            0xA4BC5DAA, 0xD2F2AB96, 0x9AEAFF61, 0xA2ED6AA8, 0x61ED3EBD, 0x9282C139, 0xB1233616, 0xE524A0B0, 0xAAA79B05, 0x339B120D, 0xDA209283, 0xFCECB025,
            0x2338D024, 0x74F295FC, 0x19E57380, 0x447D5097, 0xDB449345, 0x691DADA2, 0xE7EE1444, 0xFF877F2C, 0xF1329E38, 0xDA29BC4D, 0xFE262742, 0xA92BD2C1,
            0x0E7A42F6, 0xD17CE8CB, 0x56EC5B0F, 0x3161B769, 0x25F96DB4, 0x6D793440, 0x0BA753FA, 0xCE82A4FA, 0x614945C3, 0x8F2C450D, 0xF7604928, 0x1EC97DF3,
            0xC189D00F, 0xD3F85226, 0x14358F4D, 0x0B5F9DBA, 0x004AA907, 0x2F2622F7, 0x1FFB673E, 0xC6119CA1, 0x665D4F69, 0x90153458, 0x4654E56C, 0xD6635FAF,
            0xDF950C8A, 0xAFE40DBD, 0x4C4040BF, 0x7151F6A3, 0xF826ED29, 0xD5222885, 0xFACFBEBF, 0x517FC528, 0x076306B8, 0x298FBDEC, 0x717E55FA, 0x6632401A,
            0x9DDED4E8, 0x93FC5ED4, 0x3BD53D7A, 0x802E75CD, 0x87744F0A, 0xEA8FCC1B, 0x7CDBA99A, 0xEFE55316, 0x6EC178AB, 0x5A8972A4, 0x50702C98, 0x1FDFA1FB,
            0x44D9B76B, 0x56828007, 0x83C0BFFD, 0x5BD0490E, 0x0E6A681E, 0x2F0BC29A, 0xE1A0438E, 0xB2F60C99, 0x5E1C7AE0, 0x45A0C82C, 0x88E90B3C, 0xC696B9AC,
            0x2A83AE74, 0x65FA13BB, 0xA61F4FEB, 0xE18A8AB0, 0xB9B8E981, 0x4E1555D5, 0x9BADF245, 0x7E35C23E, 0x722E925F, 0x23685BB6, 0x0E45C66E, 0xD4873BE9,
            0xE3C041F4, 0xBE4405A8, 0x138A0FE4, 0xF437C41A, 0xEF55405A, 0x4B1D799D, 0x9C3A794A, 0xCC378576, 0xB60F3D82, 0x7E93A660, 0xC4C25CBD, 0x907FC772,
            0x10961B4D, 0x68680513, 0xFF7BC035, 0x2A438546
        },
        {
            ValidationOpCodes::Add_Shifted_Sections, ValidationOpCodes::XOR_Sections_04       , ValidationOpCodes::XOR_Sections_05,
            ValidationOpCodes::ORs_And_Shifts      , ValidationOpCodes::Sub_Shifted_Sections  , ValidationOpCodes::Add_Shifted_Sections,
            ValidationOpCodes::XOR_Sections_05     , ValidationOpCodes::Sub_Shifted_Sections  , ValidationOpCodes::XOR_Shifted_Sections,
            ValidationOpCodes::XOR_Sections        , ValidationOpCodes::ORs_And_Reverse_Shifts, ValidationOpCodes::ORs_And_Reverse_Shifts,
            ValidationOpCodes::XOR_Sections_05     , ValidationOpCodes::XOR_Sections_04       , ValidationOpCodes::ORs_And_Shifts,
            ValidationOpCodes::XOR_Sections
        }}
    };
    map.units.push_back(Unit {0, 80});
    map.units.push_back(Unit {1, 60});
    map.tiles.push_back(Tile(1));
    map.tiles.push_back(Tile(2));
    map.tiles.push_back(Tile(3));
    map.tiles.push_back(Tile(4));
    using T = std::underlying_type_t<I2Version>;
    std::ofstream out("test.bin", std::ios_base::out|std::ios_base::binary);
    out << Binary::out(map);
    out.close();
    std::cout << "map: " << std::endl;
    std::cout << Json::pretty(map) << std::endl;

    std::ifstream in("test.bin", std::ios_base::in|std::ios_base::binary);
    Map alt {};
    alt.deserialize(in);
    in.close();
    
    std::cout << "alt: " << std::endl;
    std::cout << Json::pretty(alt) << std::endl;
}
