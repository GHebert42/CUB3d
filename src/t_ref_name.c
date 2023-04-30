/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_ref_name.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/24 08:22:23 by gehebert          #+#    #+#             */
/*   Updated: 2023/04/28 21:09:19 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

t_cub	*mx_struct(t_map *m, t_cub *cub)
{
	int			k;
	int			p_box;
	const char	*chrs;

	printf("mx_struct start \n");
	chrs = cub->box.chrs;
	k = -1;
	m->mx = (t_matrx ***)calloc(sizeof(t_matrx **), m->height);
	while (++k < m->height)
		m->mx[k] = (t_matrx **)calloc(sizeof(t_matrx *), m->width);
	m->pos_y = 0;
	while (m->pos_y < m->height)
	{
		m->pos_x = 0;
		while (m->pos_x < m->width)
		{
			p_box = ft_in_set((m->m[m->pos_y][m->pos_x]), chrs);
			if (p_box != -1 || p_box == (int)ft_strlen(chrs) - 1)
				m->mx[m->pos_y][m->pos_x] = &cub->pset[p_box];
			m->pos_x++;
		}
		m->pos_y++;
	}
	printf("mx_struct exit\n");
	return (cub);
}

t_cub	*chsr_feed(t_cub *cub)
{
	int	i;
	int	j;

	j = 0;
	i = -1;
	cub->box.chrs = (char *)malloc(sizeof(char) * cub->box.chrs_len + 2);
	while (*cub->map.raw && cub->map.raw[++i] && j < cub->box.chrs_len)
	{
	
		{
			cub->box.chrs[j] = cub->map.raw[i][0];
			++j;
		}
	}
	cub->box.chrs[j++] = '@';
	cub->box.chrs[j] = '\0';
	printf("NEW CHRS {%s} len[%d]\n\n", cub->box.chrs, j);
	return (cub);
}

t_box	*e_mtrx_link(t_box *box, char **raw)
{
	char	*tex_path;
	char	*tex_name;
	int		i;

	i = -1;
	box->pnum = 0;
	box->xform = (mlx_texture_t **)calloc(sizeof(mlx_texture_t *), box->xnum + 1);
	if (!box->xform)
		return (NULL);
	while (++i < box->xnum)
	{
		if (raw[i][0] > 32)
		{
			tex_name = ft_substr(raw[i], 0, 1);
			if (raw[i][0] > 32 && raw[i][0] < 48 && raw[i][1] == 32)
				box->meta++;
			tex_path = ft_substr(raw[i], 2, ft_strlen(raw[i]) - 2);
			if (ft_in_set(raw[i][0], (const char *)MAP_NCHR) != -1)
				box->pnum++;
			box->xform[i] = mlx_load_png(tex_path);
			printf("inventaire: meta:[%d] pnum:[%d] xnum:[%d] \n\n", box->meta, box->pnum, box->xnum);
			if (!box->xform[i])
				return (report_mlx_tex_load_failed(tex_path));
			if (tex_name[0] == 'z')
			{
				box->sky = box->xform[i];
				// printf("sky_name {%s}  sky status[%d]\n\n", cub->box.chrs, );
			}
		}
	}
	return (box);
}

t_cub	*e_mtrx_count(t_cub *cub)
{
	int			i;
	const char	*rawz;

	i = -1;
	cub->box.chrs_len = 1;
	cub->box.xnum = 0;
	while (*cub->map.raw && cub->map.raw[++i])
	{
		rawz = cub->map.raw[i];
		if (rawz[0] > 32 && rawz[0] < 97 && rawz[1] == 32)
			++cub->box.chrs_len;
		if (ft_strchr_set(rawz, "_.png") != NULL)
			cub->tex.open_sky = 1; // tile
		if (ft_strchr_set(rawz, ".png") != NULL)
			++cub->box.xnum;
		printf("COUNT legende [%d] into_Raw {%s} ...\n", i, cub->map.raw[i]); 
	}
	return (cub);
}

t_cub	*e_list_txtr(t_cub *cub, t_box *box, t_map *map)
{
	box->xnum = 0;
	cub = e_mtrx_count(cub);
	printf("\nXNUM = %d ", cub->box.xnum);
	printf("___CHRS_LEN = <%d>\n", cub->box.chrs_len);
	cub->box = *e_mtrx_link(box, map->raw);
	cub = chsr_feed(cub);
	return (cub);
}
