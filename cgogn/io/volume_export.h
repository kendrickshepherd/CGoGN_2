/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* Copyright (C) 2015, IGG Group, ICube, University of Strasbourg, France       *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef CGOGN_IO_VOLUME_EXPORT_H_
#define CGOGN_IO_VOLUME_EXPORT_H_

#include <ostream>

#include <cgogn/core/utils/numerics.h>
#include <cgogn/core/utils/string.h>
#include <cgogn/core/cmap/cmap3.h>
#include <cgogn/io/io_utils.h>
#include <cgogn/io/mesh_io_gen.h>

namespace cgogn
{

namespace io
{

template <typename MAP>
class VolumeExport : public MeshExport<MAP>
{
public:
	using Inherit = MeshExport<MAP>;
	using Self = VolumeExport<MAP>;
	using Map = MAP;
	using Vertex = typename Map::Vertex;
	using Volume = typename Map::Volume;
	using ChunkArrayGen = typename Map::ChunkArrayGen;
	using ChunkArrayContainer = typename Map::template ChunkArrayContainer<uint32>;

	inline VolumeExport() :
		vertices_of_volumes_()
	  ,number_of_vertices_()
	  ,nb_tetras_(0u)
	  ,nb_pyramids_(0u)
	  ,nb_triangular_prisms_(0u)
	  ,nb_hexas_(0u)
	  ,volume_attributes_()
	{}

	virtual ~VolumeExport() override
	{}

	protected:
	virtual void prepare_for_export(Map& map, const ExportOptions& options) override
	{
		const ChunkArrayContainer& ver_cac = map.template const_attribute_container<Vertex::ORBIT>();
		const ChunkArrayContainer& vol_cac = map.template const_attribute_container<Volume::ORBIT>();

		for (const auto& pair : options.attributes_to_export_)
		{
			if (pair.first == Vertex::ORBIT)
			{
				ChunkArrayGen* ver_cag = ver_cac.get_chunk_array(pair.second);
				if (pair.second == "position")
					this->position_attribute_ = ver_cag;
				else
				{
					if (ver_cag)
						this->vertex_attributes_.push_back(ver_cag);
				}
			}
			else
			{
				ChunkArrayGen* vol_cag = vol_cac.get_chunk_array(pair.second);
				if (vol_cag)
					volume_attributes_.push_back(vol_cag);
			}
		}

		this->cell_cache_->template build<Volume>();
		number_of_vertices_.reserve(map.template nb_cells<Volume::ORBIT>());
		vertices_of_volumes_.reserve(4u* number_of_vertices_.capacity());

		uint32 count{0u};
		map.foreach_cell([&] (Vertex v) { this->indices_[v] = count++;}
		, *(this->cell_cache_));

		const auto& ids = this->indices_;
		map.foreach_cell([&] (Volume w)
		{
			uint32 nb_vert{0u};
			map.foreach_incident_vertex(w, [&nb_vert](Vertex) {++nb_vert;});
			Dart it = w.dart;

			if (nb_vert == 4u)
			{
				number_of_vertices_.push_back(4u);
				++nb_tetras_;
				vertices_of_volumes_.push_back(ids[Vertex(it)]);
				it = map.phi1(it);
				vertices_of_volumes_.push_back(ids[Vertex(it)]);
				it = map.phi1(it);
				vertices_of_volumes_.push_back(ids[Vertex(it)]);
				it = map.template phi<211>(it);
				vertices_of_volumes_.push_back(ids[Vertex(it)]);
			}
			else
			{
				if (nb_vert == 5u)
				{
					number_of_vertices_.push_back(5u);
					++nb_pyramids_;
					vertices_of_volumes_.push_back(ids[Vertex(it)]);
					it = map.phi1(it);
					vertices_of_volumes_.push_back(ids[Vertex(it)]);
					it = map.phi1(it);
					vertices_of_volumes_.push_back(ids[Vertex(it)]);
					it = map.phi1(it);
					vertices_of_volumes_.push_back(ids[Vertex(it)]);
					it = map.template phi<212>(it);
					vertices_of_volumes_.push_back(ids[Vertex(it)]);
				}
				else
				{
					if (nb_vert == 6u)
					{
						number_of_vertices_.push_back(6u);
						++nb_triangular_prisms_;
						vertices_of_volumes_.push_back(ids[Vertex(it)]);
						it = map.phi1(it);
						vertices_of_volumes_.push_back(ids[Vertex(it)]);
						it = map.phi1(it);
						vertices_of_volumes_.push_back(ids[Vertex(it)]);
						it = map.template phi<21121>(w.dart);
						vertices_of_volumes_.push_back(ids[Vertex(it)]);
						it = map.phi_1(it);
						vertices_of_volumes_.push_back(ids[Vertex(it)]);
						it = map.phi_1(it);
						vertices_of_volumes_.push_back(ids[Vertex(it)]);
					}
					else
					{
						if (nb_vert == 8u)
						{
							number_of_vertices_.push_back(8u);
							++nb_hexas_;
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.phi_1(it);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.phi_1(it);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.phi_1(it);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.template phi<21121>(w.dart);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.phi1(it);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.phi1(it);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
							it = map.phi1(it);
							vertices_of_volumes_.push_back(ids[Vertex(it)]);
						}
						else
						{
							cgogn_log_warning("VolumeExport::prepare_for_export") << "Unknown volume with " << nb_vert << " vertices. Ignoring.";
						}
					}
				}
			}
		}, *(this->cell_cache_));
	}

	inline uint32 nb_tetras() const
	{
		return nb_tetras_;
	}

	inline uint32 nb_pyramids() const
	{
		return nb_pyramids_;
	}

	inline uint32 nb_triangular_prisms() const
	{
		return nb_triangular_prisms_;
	}

	inline uint32 nb_hexas() const
	{
		return nb_hexas_;
	}

	inline std::vector<uint32> const & vertices_of_volumes() const
	{
		return vertices_of_volumes_;
	}

	inline std::vector<uint32> const & number_of_vertices() const
	{
		return number_of_vertices_;
	}

	inline std::vector<ChunkArrayGen*> const & volume_attributes() const
	{
		return volume_attributes_;
	}

private:

	void reset() override
	{
		Inherit::reset();
		vertices_of_volumes_.clear();
		number_of_vertices_.clear();
		nb_tetras_ = 0u;
		nb_pyramids_ = 0u;
		nb_triangular_prisms_ = 0u;
		nb_hexas_ = 0u;
		volume_attributes_.clear();
	}

	std::vector<uint32>	vertices_of_volumes_;
	std::vector<uint32>	number_of_vertices_;
	uint32 nb_tetras_;
	uint32 nb_pyramids_;
	uint32 nb_triangular_prisms_;
	uint32 nb_hexas_;
	std::vector<ChunkArrayGen*>	volume_attributes_;
};

#if defined(CGOGN_USE_EXTERNAL_TEMPLATES) && (!defined(CGOGN_IO_VOLUME_EXPORT_CPP_))
extern template class CGOGN_IO_API VolumeExport<CMap3<DefaultMapTraits>>;
#endif // defined(CGOGN_USE_EXTERNAL_TEMPLATES) && (!defined(CGOGN_IO_VOLUME_EXPORT_CPP_))

} // namespace io

} // namespace cgogn

#endif // CGOGN_IO_VOLUME_EXPORT_H_
