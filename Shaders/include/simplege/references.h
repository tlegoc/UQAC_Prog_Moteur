#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class BinAsset;
  using BinAssetRef = std::shared_ptr<BinAsset>;

  class Entity;
  using EntityRef = std::shared_ptr<Entity>;

  class Image;
  using ImageRef = std::shared_ptr<Image>;

  class TextAsset;
  using TextAssetRef = std::shared_ptr<TextAsset>;

  namespace Network
  {
    class Connection;
    using ConnectionRef = std::shared_ptr<Connection>;

    class Server;
    using ServerRef = std::shared_ptr<Server>;
  } // namespace Network
} // namespace SimpleGE