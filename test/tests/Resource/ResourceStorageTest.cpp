#include "Resource.hpp"

class Texture
{
public:

	int dsdsds = 0;

};

class PngLoader :public res::IResourceLoader<Texture>
{
public:
	PngLoader(int aaaa)
		:aaa_(aaaa)
	{

	}
	virtual ~PngLoader()override final
	{

	}
	virtual bool Load(void* ptr) override
	{
		Texture* tex = static_cast<Texture*>(ptr);
		tex->dsdsds = aaa_;
		return true;
	}


	int aaa_;
};

TEST(ResourceStorageTest, Test)
{
	res::ResourceRegistry regi;
	res::ResourceStorage storage(regi);

	storage.Load<Texture, PngLoader>(nullptr, 100);
	storage.Load<Texture, PngLoader>(nullptr, 100);
	storage.Load<Texture, PngLoader>(nullptr, 100);
	storage.Load<Texture, PngLoader>(nullptr, 100);
	res::ResourceID id = storage.Load<Texture, PngLoader>(nullptr, 100);

	{
		res::ResourceView<Texture> view1 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view2 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view3 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view4 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view5 = storage.Look<Texture>(id);

		const Texture* pTex = view1.Get();

		EXPECT_EQ(100, pTex->dsdsds);
	}


	{
		res::ResourceView<Texture> view1 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view2 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view3 = storage.Look<Texture>(id);
		storage.ReLoad(id);
		res::ResourceView<Texture> view4 = storage.Look<Texture>(id);
		res::ResourceView<Texture> view5 = storage.Look<Texture>(id);

		const Texture* pTex = view1.Get();

		EXPECT_EQ(100, pTex->dsdsds);
	}
}
