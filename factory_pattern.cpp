#include <typeinfo>
#include <memory>
#include <map>
#include <functional>
#include <string>
#include <string_view>

using namespace std::string_literals;

class Image {};

class BitmapImage : public Image {};
class PngImage : public Image {};
class JpgImage : public Image {};

struct IImageFactory
{
    virtual std::unique_ptr<Image> Create(std::string_view type) = 0;
};


struct ImageFactory : public IImageFactory
{
    virtual std::unique_ptr<Image> Create(std::string_view type) override
    {
        static std::map<std::string, std::function<std::unique_ptr<Image>()>> mapping
        {
        { "bmp", []() {return std::make_unique<BitmapImage>(); } },
        { "png", []() {return std::make_unique<PngImage>(); } },
        { "jpg", []() {return std::make_unique<JpgImage>(); } }
        };

        auto it = mapping.find(type.data());
        if (it != mapping.end())
        return it->second();

        return nullptr;
    }
};
