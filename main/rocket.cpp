#include "rocket.hpp"
#include "../vmlib/mat33.hpp"

SimpleMeshData make_cone(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
    std::vector<Vec3f> pos;

    for (std::size_t i = 0; i < aSubdivs; ++i) {
        float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
        float y = std::cos(angle);
        float z = std::sin(angle);
        float prevY = std::cos(i / float(aSubdivs) * 2.f * 3.1415926f);
        float prevZ = std::sin(i / float(aSubdivs) * 2.f * 3.1415926f);

        // triangle for side
        pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
        pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
        pos.emplace_back(Vec3f{ 1.f, y, z });
    }

    if (aCapped) {
        for (std::size_t i = 0; i < aSubdivs; ++i) {
            float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
            float y = std::cos(angle);
            float z = std::sin(angle);
            float prevY = std::cos(i / float(aSubdivs) * 2.f * 3.1415926f);
            float prevZ = std::sin(i / float(aSubdivs) * 2.f * 3.1415926f);

            // bottom cap
            pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
            pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
            pos.emplace_back(Vec3f{ 1.f, y, z });
        }
    }

    for (auto& p : pos) {
        Vec4f p4{ p.x, p.y, p.z, 1.f };
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{ t.x, t.y, t.z };
    }

    std::vector col(pos.size(), aColor);

	SimpleMeshData meshData;
	meshData.vertex_positions = std::move(pos);
	meshData.colors = std::move(col);

	return meshData;

}

SimpleMeshData make_cylinder(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
	std::vector<Vec3f> pos;
	std::vector<Vec3f> norms;
	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	Mat33f N = mat44_to_mat33(transpose(invert(aPreTransform)));

	for (std::size_t i = 0; i < aSubdivs; ++i) {
		float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
		float y = std::cos(angle);
		float z = std::sin(angle);

		Vec3f normal = Vec3f{ 0, y, z };
		Vec3f transformedNormal = N * normal;

		// add triangle for side
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		norms.emplace_back(transformedNormal);
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		norms.emplace_back(transformedNormal);
		pos.emplace_back(Vec3f{ 1.f, y, z });
		norms.emplace_back(transformedNormal);
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		norms.emplace_back(transformedNormal);
		pos.emplace_back(Vec3f{ 1.f, y, z });
		norms.emplace_back(transformedNormal);
		pos.emplace_back(Vec3f{ 0.f, y, z });

		prevY = y;
		prevZ = z;
	}

	if (aCapped) {
		// draw bottom cap
		for (std::size_t i = 0; i < aSubdivs; ++i) {
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
			float y = std::cos(angle);
			float z = std::sin(angle);
			Vec3f normal = Vec3f{ -1, 0, 0 };
			Vec3f transformedNormal = N * normal;

			// bottom cap
			pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
			norms.emplace_back(transformedNormal);
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			norms.emplace_back(transformedNormal);
			pos.emplace_back(Vec3f{ 0.f, y, z });
			norms.emplace_back(transformedNormal);

			// top cap
			normal = Vec3f{ 1, 0, 0 };
			transformedNormal = N * normal;
			pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			norms.emplace_back(transformedNormal);
			pos.emplace_back(Vec3f{ 1.f, y, z });
			norms.emplace_back(transformedNormal);
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			norms.emplace_back(transformedNormal);

			prevY = y;
			prevZ = z;
		}
	}

	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		p = Vec3f{ t.x, t.y, t.z };
	}

	std::vector col(pos.size(), aColor);

	SimpleMeshData meshData;
	meshData.vertex_positions = std::move(pos);
	meshData.colors = std::move(col);
	meshData.vertex_normals = std::move(norms);

	return meshData;

}
