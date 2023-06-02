/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_actions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 18:25:58 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/01 22:18:55 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

#define PORTAL_TRIGGER_DIST_SQ 100.0f

int	__obj_action_player(t_oinst *obj, t_cub *cub)
{
//	static int	counter;
//	t_hero		*player;
//	float		pos[4];
	(void)obj;
	(void)cub;
//	if (obj != cub->hero.ply_obj)
//	{
		
//	}
	
//	if (obj->isactive)
//	{
		// if (counter > 1000)
		// {
		// 	player = (t_hero *)obj->relative;
		// 	pos[0] = obj->px;
		// 	pos[1] = obj->py;
		// 	pos[2] = (*player->dirx) * 10.0f ;
		// 	pos[3] = (*player->diry) * 10.0f ;
		// 	create_obj_instance(cub, pos, OBJ_FIREBALL,
		// 		player->ply_obj->allegiance, NULL);
		// 	counter = 0;
		// }
		// ++counter;
//	}
	return (0);
}

int	__obj_action_spawnpoint(t_oinst *obj, t_cub *cub)
{
//	t_oinst		*player;
//	int			i;

	if (!obj->isactive)
		return (-1);
	(void)cub;
//	printf("nb players : %d\n", cub->nb_players);

	/// This updates the players spawn point to the one placed in its current
	/// cell if spawnpoint is active and of same allegience.
	/*
	i = -1;
	while (++i < cub->nb_players)
	{
		player = get_obj(cub, i);
		if (player && player->allegiance == obj->allegiance
			&& player->cx == obj->cx && player->cy == obj->cy)
			player->spawnpoint = obj;
	}
	*/
	return (0);
}

int	__obj_action_portal(t_oinst *obj, t_cub *cub)
{
	float	dx;
	float	dy;
	float	dist;
	t_oinst	*link;
	
	if (!obj->isactive || !obj->relative)
		return (-1);
	link = obj->relative;
	dx = obj->px - cub->hero.ply_obj->px;
	dy = obj->py - cub->hero.ply_obj->py;
	dist = dx * dx + dy * dy;
	if (dist < PORTAL_TRIGGER_DIST_SQ)
	{
		cub->hero.ply_obj->px = link->px + dx * 1.5f;
		cub->hero.ply_obj->py = link->py + dy * 1.5f;
		return (0);
	}
	else
		return (-1);
}

int	__fireball_check_hit(t_cub *cub, t_oinst *obj)
{
	float	closest_plyr;
	t_oinst	*other;
	t_oinst	*target;
	float	delta[2];
	float	dist;	

//	printf("fireball is_wall (%d, %d) : %d\n", (int)(obj->px * cub->inv_cw), (int)(obj->py * cub->inv_cw), is_wall(&cub->map, obj->px * cub->inv_cw, obj->py * cub->inv_cw));
	if (is_wall(&cub->map, (obj->px + obj->dx * 10.0f) * cub->inv_cw,
		(obj->py + obj->dy * 10.0f) * cub->inv_cw))
		return (delete_oinst_by_id(cub, obj->_id));
	other = cub->objs.instances;
	closest_plyr = INFINITY;
	while (other)
	{
		if (other->type->type_enum != OBJ_PLAYER
			|| other->allegiance == obj->allegiance)
		{
			other = other->next;
			continue ;
		}
		delta[0] = other->px - obj->px;
		delta[1] = other->py - obj->py;
		dist = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);
		printf("fireball dist : %f\n", dist);
		if (dist < 10)
		{
			respawn_player(other);
			return (delete_oinst_by_id(cub, obj->_id));
		}
		else if (obj->relative && dist < closest_plyr)
		{
			obj->dx = delta[0] / dist;
			obj->dy = delta[1] / dist;
			closest_plyr = dist;
			target = other;
		}
		other = other->next;
	}
	if (obj->relative && target != obj->relative)
	{
		link_fireball_to_target(obj, target);
//			obj->px += obj->dx * obj->type->speed;
//			obj->py += obj->dy * obj->type->speed;
	}
//	else
//		obj_move_abs(cub, obj, obj->dx, obj->dy);
	return (0);
}

int	__obj_action_fireball(t_oinst *obj, t_cub *cub)
{
//	float	dist;
//	t_oinst	*other;
//	float	delta_prtl[2];
	
//	printf("FIREBAAAAAAAAALLLL\n");
	if (!obj->isactive)
		return (-1);
	if (__fireball_check_hit(cub, obj) == 0)
		obj_move_abs(cub, obj, obj->dx, obj->dy);
/*
	if (obj->relative)
	{
//		player = (t_hero *)obj->relative;
		other = (t_oinst *)obj->relative;

		obj->dx = other->px - obj->px;
		obj->dy = other->py - obj->py;
		dist = sqrtf(obj->dx * obj->dx + obj->dy * obj->dy);
		if (dist < 10)
		{
//			ft_printf("OUCH MAUDIT CA FAIT MAL!\n");
			respawn_player((t_oinst *)obj->relative);
			delete_oinst_by_id(cub, obj->_id);
			return (0);
		}
		obj->dx /= dist;
		obj->dy /= dist;
		obj->px += obj->dx * obj->type->speed;
		obj->py += obj->dy * obj->type->speed;
	}
	else
	{
		other = cub->objs.instances;
		while (other)
//		{
			
//		}
		obj->px += obj->dx * obj->type->speed;
		obj->py += obj->dy * obj->type->speed;
	}
*/
	/*
	objs = cub->objs.instances;
	while (objs)
	{
		if (objs->type->type_enum == OBJ_PORTAL && objs->isactive)
		{
			delta_prtl[0] = objs->px - obj->px;
			delta_prtl[1] = objs->py - obj->py;
			if (delta_prtl[0] * delta_prtl[0] + delta_prtl[1] * delta_prtl[1]
				< PORTAL_TRIGGER_DIST_SQ)
			{
				obj->px = ((t_oinst *)objs->relative)->px + delta_prtl[0] * 1.5f;
				obj->py = ((t_oinst *)objs->relative)->py + delta_prtl[1] * 1.5f;
				break ;
			}
		}
		objs = objs->next;
	}
	*/
	return (0);
}

int	__obj_action_firepit(t_oinst *obj, t_cub *cub)
{
	float		pos[4];
	int			inst_id;
	
	if (!obj->isactive)
		return (-1);
	printf("firepit active\n");
	if (++obj->counter > FIREPIT_SPAWN_TICKS)
	{
		pos[0] = obj->px;
		pos[1] = obj->py;
		pos[2] = obj->dx;
		pos[3] = obj->dy;
		printf("SPAWNING FIREBALL : pos (%f, %f), dir (%f, %f)\n", pos[0], pos[1], pos[2], pos[3]);
		inst_id = create_obj_instance(cub, pos, OBJ_FIREBALL,
			obj->allegiance, obj->relative);
		activate_fireball(get_obj(cub, inst_id), 1, NULL);
		obj->counter = 0;
	}
	return (0);
}

int	__obj_action_lever(t_oinst *obj, t_cub *cub)
{
	t_oinst	*prtl;
//	int		cx;
//	int		cy;

	if (obj->isactive)
	{
		prtl = (t_oinst *)obj->relative;
		if (obj->counter > 600)
		{
			activate_portal((t_oinst *)obj->relative, 0);
			obj->isactive = 0;
			obj->special_gset.xwalls[0] = obj->gset->xwalls[0];
			obj->counter = 0;
		}
		++obj->counter;
	}
	if (obj->relative)
	{
//		ft_eprintf("lever relative exists\n");
//		cx = (int)obj->px;
//		cy = (int)obj->py;
//		ft_eprintf("lever cx, cy (%d, %d), hero cx, cy (%d, %d)\n", cx, cy, 
//			cub->hero.cell_x, cub->hero.cell_y);
		prtl = (t_oinst *)obj->relative;
		if (!(cub->hero.ply_obj->cx == obj->cx
			&& cub->hero.ply_obj->cy == obj->cy))
			return (-1);
		ft_eprintf("PRESSED !\n");
		activate_portal(prtl, 1);
		obj->isactive = 1;
		obj->special_gset.xwalls[0] = obj->gset->xwalls[1];
//		dual = cub->map.mx[cy][cx];
//		dual->xwalls[0] = obj->type->gset->xwalls[1];
	}
//	else
//		printf("Lever has no relative\n");
	return (0);
}

// Attempts to commit an action with every object currently in world.
// Every obj inst is responsible for verifying if its action is possible and valid.
void    commit_all_obj_actions(t_cub *cub)
{
	t_oinst *obj;

	obj = cub->objs.instances;
	while (obj)
	{
		if (obj->action)
			obj->action(obj, cub);

		obj = obj->next;
	}
}
