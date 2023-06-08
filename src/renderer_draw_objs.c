/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer_draw_objs.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 10:21:23 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/08 04:29:36 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// Assumes is_drawable == true, checked earlier.
inline void	__rdr_select_draw_texture(t_objd *od, t_oinst *obj)
{
	const float		rad_to_idx_ratio = 8.0f * M_INV_TAU;
	float			dpos[2];
	float			rel_ori;
	int				idx;

	if (!obj)
		return ;
	od->tex = NULL;
	if (obj->type->is_oriented)
	{
		dpos[0] = obj->px - od->ppos[0];
		dpos[1] = obj->py - od->ppos[1];
		rel_ori = atan2f(dpos[1], dpos[0]) - obj->ori + FOV45_HF;
		while (rel_ori < 0)
			rel_ori += M_TAU;
		idx = (int)(rel_ori * rad_to_idx_ratio) % 8;
		od->tex = obj->gset->xwalls[idx];
	}
	else
		od->tex = obj->gset->xwalls[obj->tex_idx];
	od->pxls = (uint32_t *)(od->tex->pixels);
}

inline void	__rdr_setup_draw_objects(t_cub *cub, t_objd *od, int *pframe, int offy)
{
	od->tincrs[0] = (float)od->tex->width / (float)od->dims[0];
	od->tincrs[1] = (float)od->tex->height / (float)od->dims[1];
	*((size_t *)od->toffs) = 0;
	od->loffs[0] = od->drawx - (od->dims[0] >> 1);
	od->loffs[1] = cub->scn_midy - (od->dims[1] >> 1)
		+ (int)(offy * od->ratio);
	od->loffs[2] = od->loffs[0] + od->dims[0];
	od->loffs[3] = od->loffs[1] + od->dims[1];
	if (od->loffs[0] < pframe[0])
	{
		od->toffs[0] = pframe[0] - od->loffs[0];
		od->dims[0] -= od->toffs[0];
		od->loffs[0] = pframe[0];
	}
	if (od->loffs[1] < pframe[1])
	{
		od->toffs[1] = od->pframe[1] - od->loffs[1];
		od->dims[1] -= od->toffs[1];
		od->loffs[1] = pframe[1];
	}
	if (od->loffs[2] > pframe[2])
		od->dims[0] -= (od->loffs[2] - pframe[2]);
	if (od->loffs[3] > pframe[3])
		od->dims[1] -= (od->loffs[3] - pframe[3]);
	od->loffs[2] = ft_clamp(od->loffs[2], pframe[0], pframe[2]);
	od->loffs[3] = ft_clamp(od->loffs[3], pframe[1], pframe[3]);
}

inline void	__rdr_obj_draw_check(t_cub *cub, t_objd *od)
{
	od->dims[0] = 0;
	if (!od->obj->type->is_drawable || !od->obj->gset)
//		|| od->obj == cub->hero.ply_obj)
		return ;
	od->ov[0] = od->obj->px - od->ppos[0];//cub->hero.ply_obj->px;
	od->ov[1] = od->obj->py - od->ppos[1];//cub->hero.ply_obj->py;
	od->odist = (*cub->hero.dirx) * od->ov[0] + (*cub->hero.diry) * od->ov[1];
	od->obj->dist = od->odist;
//	od->tex = select_draw_texture(cub, od->obj);
	od->ratio = cub->near_z / od->odist;
	od->drawx = (int)(((*cub->hero.dirx) * od->ov[1] - (*cub->hero.diry)
				* od->ov[0]) * od->ratio) + cub->scn_midx;
	od->dims[0] = (int)(od->ratio * od->obj->type->width);
	od->dims[1] = (int)(od->ratio * od->obj->type->height);
	return ;
}



static inline void	adjust_draw_frame(t_cub *cub, t_objd *od)
{
	int	i;

	od->rd = od->rdata + od->loffs[0];
	i = 0;
	while (i < od->dims[0] && od->rd[i].dist < od->odist)
		++i;
	od->toffs[0] += i;
	od->dims[0] -= i;
	od->loffs[0] += i;
	od->rd += i;
	od->x = -1;
	while (++od->x < od->dims[0] && od->odist <= od->rd[od->x].dist)
		od->tys[od->x] = (int)(od->x * od->tincrs[0]);
	od->dims[0] = od->x;
	if (od->x == 0)
		return ;
	od->loffs[2] = od->loffs[0] + od->x;
	od->so = od->loffs[0] + od->loffs[1] * SCN_WIDTH - 1;
	od->bincr = SCN_WIDTH - od->dims[0];
//	od->pxls = ((uint32_t *)od->tex->pixels)
	od->pxls += (int)(od->toffs[0] * od->tincrs[0])
		+ ((int)(od->toffs[1] * od->tincrs[1]) * od->tex->width);
	od->dbuff = ((float *)cub->renderer.dbuff) + od->so;
	od->pbuff = ((uint32_t *)cub->renderer.objs_layer->pixels) + od->so;
}

void	__render_obj(t_cub *cub, t_objd *od)
{
	uint32_t	*tb;
	int			i;
	int			j;

	adjust_draw_frame(cub, od);
	if (od->dims[0] == 0)
		return ;
	j = -1;
	while (++j < od->dims[1])
	{
		tb = od->pxls + (int)(j * od->tincrs[1]) * od->tex->width;
		i = -1;
		while (++i < od->dims[0])
		{
			++od->dbuff;
			++od->pbuff;
			od->cl = tb[od->tys[i]];
			if (!od->cl || (*od->dbuff && (*od->dbuff < od->odist)))
				continue ;
			*od->pbuff = od->cl;
			*od->dbuff = od->odist;
		}
		od->dbuff += od->bincr;
		od->pbuff += od->bincr;
	}
}

static inline void	__label_isproj(uint32_t *pbuff, char *isproj, int *pframe, int *pdims)
{
	const int		start_offset = pframe[0] + (pframe[1] * SCN_WIDTH);
	const int		buff_jump = SCN_WIDTH - pdims[0];
	int		w;
	int		h;

	pbuff += start_offset - 1;
	isproj += start_offset - 1;
	h = pdims[1];
	while (h--)
	{
		w = pdims[0];
		while (w--)
		{
			++isproj;
			if (*(++pbuff) == PROJ_COLOR)
				*isproj = 1;
		}
		pbuff += buff_jump;
		isproj += buff_jump;			
	}
}


static inline void	init_draw_objects(t_cub *cub, t_objd *od)
{
	memset(cub->renderer.objs_layer->pixels, 0, sizeof(uint32_t)
		* SCN_WIDTH * SCN_HEIGHT);
	memset(cub->renderer.dbuff, 0, sizeof(float) * SCN_WIDTH * SCN_HEIGHT);
	od->pframe = cub->renderer.pframe;
//	od->pframe[0] = cub->renderer.bframe[0];
//	od->pframe[1] = cub->renderer.bframe[1];
//	od->pframe[2] = cub->renderer.bframe[2];
//	od->pframe[3] = cub->renderer.bframe[3];
	od->rdata = cub->hero.rcast.rdata;
	od->pdata = cub->hero.rcast.pdata;
	od->isproj = cub->renderer.isproj;
	od->obj = cub->objs.instances;
	od->ppos[0] = cub->hero.ply_obj->px;
	od->ppos[1] = cub->hero.ply_obj->py;
}

void	render_objects(t_cub *cub)
{
	t_objd	od;
	
	init_draw_objects(cub, &od);
	while (od.obj)
	{
		if (od.obj == cub->hero.ply_obj && next_obj(&od.obj))
			continue ;
		__rdr_obj_draw_check(cub, &od);

		if ((!od.dims[0] || (od.odist <= 1.0f) || (od.drawx + (od.dims[0] >> 1)) < 0
			|| SCN_WIDTH <= (od.drawx - (od.dims[0] >> 1))) && next_obj(&od.obj))
			continue ;

		__rdr_select_draw_texture(&od, od.obj);
		
		od.pframe[0] = cub->renderer.bframe[0];
		od.pframe[1] = cub->renderer.bframe[1];
		od.pframe[2] = cub->renderer.bframe[2];
		od.pframe[3] = cub->renderer.bframe[3];
		__rdr_setup_draw_objects(cub, &od, od.pframe, od.obj->type->offy);
		__render_obj(cub, &od);


		if (obj_get_type(od.obj) == OBJ_PORTAL && od.obj->isactive)
		{
//			int	*pframe = cub->renderer.pframe;
//			int	pdims[2] = {(int)(obj->type->proj_width * ratio), (int)(obj->type->proj_height * ratio)};
			od.dims[0] = (int)(od.obj->type->proj_width * od.ratio);
			od.dims[1] = (int)(od.obj->type->proj_height * od.ratio);

			od.pframe[0] = od.drawx - (od.dims[0] >> 1);
			od.pframe[2] = ft_clamp(od.pframe[0] + od.dims[0], 0, SCN_WIDTH);
			od.pframe[0] = ft_clamp(od.pframe[0], 0, SCN_WIDTH);

			if ((od.pframe[0] == od.pframe[2]) && next_obj(&od.obj))
			{
				printf("BAILOUT ON PROJECTION !!\n");
				continue ;
			}
			od.pframe[1] = cub->scn_midy - (od.dims[1] >> 1)
				+ (od.ratio * od.obj->type->offy);
			od.pframe[3] = ft_clamp(od.pframe[1] + od.dims[1], 0, SCN_HEIGHT);
			od.pframe[1] = ft_clamp(od.pframe[1], 0, SCN_HEIGHT);
			od.dims[0] = od.pframe[2] - od.pframe[0];
			od.dims[1] = od.pframe[3] - od.pframe[1];
			
			printf("pframe : [%d, %d, %d, %d], dims : [%d, %d]\n",
				od.pframe[0], od.pframe[1], od.pframe[2], od.pframe[3],
				od.dims[0], od.dims[1]);
			memset(cub->renderer.dpbuff, 0, sizeof(float) * SCN_WIDTH * SCN_HEIGHT);
			memset(cub->renderer.isproj, 0, sizeof(char) * SCN_WIDTH * SCN_HEIGHT);
			
			cub->renderer.portal = od.obj;
			__label_isproj((uint32_t *)cub->renderer.objs_layer->pixels,
				od.isproj, od.pframe, od.dims);
			prtl_proj_vectors(od.pdata + od.pframe[0] - 1,
				&cub->map, od.obj, od.pframe[2] - od.pframe[0]);
			order_draw_call(cub->draw_threads, 3, 6);
		}

		od.obj = od.obj->next;
	}
}

