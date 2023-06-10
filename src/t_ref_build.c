/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_ref_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gehebert <gehebert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 21:34:03 by gehebert          #+#    #+#             */
/*   Updated: 2023/06/10 00:19:57 by gehebert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

t_matrx	*gset_builder(const char *path, int txtr_nb)
{
	t_matrx	*gset;
	char	*arr_name;
	char	*name;
	int		i;

	gset = (t_matrx *)malloc(sizeof(t_matrx));
	if (!gset)
		return (NULL);
	i = 0;
	printf("GSET    %d txtrs >>>> *%s Model >>> \n", txtr_nb, path);
	while (i < txtr_nb)
	{
		name = ft_strjoin(path, ft_itoa(i));
		arr_name = ft_strjoin((const char *)name, ".png");
		gset->xwalls[i] = mlx_load_png(arr_name);
		if (!gset->xwalls[i])
			return (report_mlx_tex_load_failed(arr_name));
		printf("GSET <<%p>> [%d] >>> %s []  \n", gset->xwalls[i], i, arr_name);
		i++;
		free(name);
		free(arr_name);
	}
	return (gset);
}

t_cub	*dual_builder(t_cub *cub, int i, char *t_name)
{
	char	**tex_set;

	if (cub->box.n_dual > i)
	{
		tex_set = ft_split_space(t_name);
		cub->dual[i].xwalls[1] = NULL;
		cub->dual[i].xwalls[0] = mlx_load_png(tex_set[0]);
		if (!cub->dual[i].xwalls[0])
			return (report_mlx_tex_load_failed(tex_set[0]));
		printf("DUAL [%d][0] << %p >> [%d] >>> %s []\n", i, cub->dual[i].xwalls[0], i, tex_set[0]);
		if (tex_set[1])
		{
			cub->dual[i].xwalls[1] = mlx_load_png(tex_set[1]);
			if (!cub->dual[i].xwalls[1])
				return (report_mlx_tex_load_failed(tex_set[1]));
			printf("DUAL [%d][1]<< %p >> [%d] >>> %s []\n", i, cub->dual[i].xwalls[1], i, tex_set[1]);
		}
		// cub->box.xnum--;
	}
	return (cub);
}

t_cub	*meta_builder(t_cub *cub, t_box *box, char *t_name, t_objs *objs)
{
	int	head;

	head = ft_in_set(t_name[0], (const char *)MCHR);
	if (ft_in_set(t_name[0], (const char *)MOD_LEV) != -1
		&& (box->n_lvls++ < 1))
		init_lever_model(objs);
	else if (ft_in_set(t_name[0], (const char *)MOD_PORT) != -1
		&& box->n_prts++ < 1)
		init_portal_model(objs);
	else if (head > 11 && head < 18)
	{
		if (head > 11 && box->n_fbll++ < 1)
			init_fireball_model(objs);
		if (head < 15 && box->n_fpit++ < 1)
			init_firepit_model(objs);
	}
	else if (ft_in_set(t_name[0], (const char *)MOD_SPEC) != -1)
	{
		if (box->n_plyr == 0)
			init_player_model(objs);
		box->n_plyr++;
	}
	return (cub);
}

t_matrx	***init_mx(t_map *m)
{
	int	k;

	k = -1;
	m->mx = (t_matrx ***)calloc(sizeof(t_matrx **), m->height);
	while (++k < m->height)
		m->mx[k] = (t_matrx **)calloc(sizeof(t_matrx *), m->width);
	return (m->mx);
}

/// MAPX BUILDER
t_cub	*mapx_builder(t_map *m, t_cub *cub)
{
	int			p_box;
	int			grim;
	const char	*chrs;

	chrs = cub->box.chrs;
	grim = ((int)ft_strlen(chrs) - 1) - cub->box.pset;
	m->pos_y = 0;
	while (m->pos_y < m->height)
	{
		m->pos_x = 0;
		while (m->pos_x < m->width)
		{
			p_box = ft_in_set((m->m[m->pos_y][m->pos_x]), chrs);
			if ((p_box >= grim && (p_box < (int)ft_strlen(chrs) - 1)))
				m->mx[m->pos_y][m->pos_x] = &cub->pset[p_box - grim];
			else if ((p_box < grim) && p_box > (grim - cub->box.n_dual - 1))
				m->mx[m->pos_y][m->pos_x] = &cub->dual[p_box - cub->box.meta + 1];
			m->pos_x++;
		}
		m->pos_y++;
	}
	mapx_alt_pos(m, cub, p_box);
	return (cub);
}
