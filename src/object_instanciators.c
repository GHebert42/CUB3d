/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_instanciators.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 20:45:55 by iamongeo          #+#    #+#             */
/*   Updated: 2023/06/02 22:00:07 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

// pos is in world coord, NOT cell coord.
int	create_player_instance(t_cub *cub, float *pos, int allegiance,\
 t_oinst *spawnp)
{
	t_oinst	*new_obj;

	if (!spawnp)
		return (report_err("Creating a player instance must provide ptr to a spawnpoint obj inst.\n"));
//	printf("player type_enum at createplayer inst start: %d\n", cub->objs.player.type_enum);
	if (!cub->objs.player.type_enum)
		init_player_model(&cub->objs);
	new_obj = NULL;
	if (!ft_malloc_p(sizeof(t_oinst), (void **)&new_obj))
		return (report_malloc_error());
//	printf("create_player_instance : Creating player at (%f, %f)\n",
//		pos[0], pos[1]);
	new_obj->type = &cub->objs.player;
//	printf("is_drawable and is_oriented at player instanciation : %d, %d\n", 
//		new_obj->type->is_drawable, new_obj->type->is_oriented);
	new_obj->_id = get_new_obj_id();
	new_obj->allegiance = allegiance;
	new_obj->tex_idx = 0;
	new_obj->spawnpoint = spawnp;
	obj_set_position(cub, new_obj, pos[0], pos[1]);
	obj_set_orientation(cub, new_obj, spawnp->ori);
	new_obj->action = __obj_action_player;
	new_obj->counter = 0;
	new_obj->rel_type_enum = 0;
	new_obj->relative = spawnp;
	new_obj->isactive = 0;
	new_obj->gset = new_obj->type->gsets[new_obj->allegiance];
	new_obj->next = cub->objs.instances;
	cub->objs.instances = new_obj;
//	printf("Single Player instance created at pos (%f, %f)\n", pos[0], pos[1]);
//	printf("player type_enum at createplayer inst end: %d\n", cub->objs.player.type_enum);
	return (new_obj->_id);	
}

// pos is in world coord, NOT cell coord.
int	create_spawnp_instance(t_cub *cub, float *pos, int allegiance)
{
	t_oinst	*new_obj;

	if (!cub->objs.spawnp.type_enum)
		init_spawnpoint_model(&cub->objs);
	new_obj = NULL;
	if (!ft_malloc_p(sizeof(t_oinst), (void **)&new_obj))
		return (report_malloc_error());
//	printf("create_spawnpoint_instance : Creating spawnp at (%f, %f)\n",
//		pos[0], pos[1]);
	new_obj->type = &cub->objs.spawnp;
	new_obj->_id = get_new_obj_id();
	new_obj->allegiance = allegiance;
	new_obj->tex_idx = 0;
	obj_set_position(cub, new_obj, pos[0], pos[1]);
	new_obj->ori = 0.0f;
	new_obj->action = __obj_action_spawnpoint;
//	new_obj->rel_type_enum = 0;
	new_obj->relative = cub;
	new_obj->isactive = 1;
	new_obj->gset = NULL;
//	new_obj->gset = new_obj->type->gsets[new_obj->allegiance];
	new_obj->next = cub->objs.instances;
	cub->objs.instances = new_obj;
//	printf("Single Spawnpoint instance created at pos (%f, %f)\n", pos[0], pos[1]);
	return (new_obj->_id);	
}

int	create_lever_instance(t_cub *cub, float *pos, int allegiance, t_oinst *link)
{
	t_oinst	*new_obj;
	int	cell[2];

	if (!ft_malloc_p(sizeof(t_oinst), (void **)&new_obj))
		return (report_malloc_error());
	new_obj->type = &cub->objs.lever;
	new_obj->_id = get_new_obj_id();
	new_obj->tex_idx = 0;
	new_obj->allegiance = allegiance;
	obj_set_position(cub, new_obj, pos[0], pos[1]);
	cell[0] = new_obj->cx;
	cell[1] = new_obj->cy;
	new_obj->relative = link;
	new_obj->isactive = 0;	
	new_obj->action = __obj_action_lever;
	new_obj->counter = 0;
	new_obj->gset = new_obj->type->gsets[0];
	new_obj->next = cub->objs.instances;
	cub->objs.instances = new_obj;
	if (cub->map.mx[cell[1]][cell[0]])
		new_obj->special_gset.xwalls[1] = cub->map.mx[cell[1]][cell[0]]->xwalls[1];
	else
		new_obj->special_gset.xwalls[1] = cub->dual[0].xwalls[1];
	new_obj->special_gset.xwalls[0] = new_obj->type->gsets[0]->xwalls[0];
	cub->map.mx[cell[1]][cell[0]] = &new_obj->special_gset;
	return (new_obj->_id);
}

// pos is in world coord, NOT cell coord.
int	create_portal_instance(t_cub *cub, float *pos, int allegiance, t_oinst *link)
{
	t_oinst	*new_obj;

	if (link && link->type->type_enum != OBJ_PORTAL)
		return (report_err("Creating a portal instance trying to link to non portal obj.\n"));
	new_obj = NULL;
	if (!ft_malloc_p(sizeof(t_oinst), (void **)&new_obj))
		return (report_malloc_error());
	new_obj->type = &cub->objs.portal;
	new_obj->_id = get_new_obj_id();
	new_obj->allegiance = allegiance;
	new_obj->tex_idx = 0;
	obj_set_position(cub, new_obj, pos[0], pos[1]);
	new_obj->action = __obj_action_portal;
	new_obj->rel_type_enum = 0;
	new_obj->relative = new_obj;
	new_obj->counter = 0;
	new_obj->isactive = 0;
	if (link && link->type->type_enum == OBJ_PORTAL)
	{
		new_obj->rel_type_enum = OBJ_PORTAL;
		new_obj->relative = link;
		link->relative = new_obj;
	}
	new_obj->gset = new_obj->type->gsets[0];
	new_obj->next = cub->objs.instances;
	cub->objs.instances = new_obj;
	return (new_obj->_id);	
}

// pos is in world coord, NOT cell coord.
int	create_fireball_instance(t_cub *cub, float *pos, int allegiance, t_oinst *link)
{
	t_oinst	*new_obj;

	if (!cub->objs.fireball.type_enum)
		init_fireball_model(&cub->objs);
	if (!ft_malloc_p(sizeof(t_oinst), (void **)&new_obj))
		return (report_malloc_error());

	new_obj->type = &cub->objs.fireball;
	new_obj->_id = get_new_obj_id();
	new_obj->allegiance = allegiance;
	new_obj->tex_idx = allegiance;
	obj_set_position(cub, new_obj, pos[0], pos[1]);
	new_obj->dx = pos[2];
	new_obj->dy = pos[3];
	new_obj->relative = NULL;
	new_obj->action = __obj_action_fireball;
	if (link)
	{
//		printf("linking fireball %d (%p) to portal %d (%p)\n", new_obj->_id, new_obj, link->_id, link);
		new_obj->relative = link;
		new_obj->isactive = 1;
	}
	else
		new_obj->isactive = 0;
	new_obj->gset = new_obj->type->gsets[0];
	new_obj->next = cub->objs.instances;
	cub->objs.instances = new_obj;
//	printf("Single fireball instance created at pos (%f, %f)\n", pos[0], pos[1]);
	return (new_obj->_id);	
}

//pos is in world coord, NOT cell coord.
int	create_firepit_instance(t_cub *cub, float *pos, int allegiance, t_oinst *link)
{
	t_oinst	*new_obj;

	if (!cub->objs.firepit.type_enum)
		init_firepit_model(&cub->objs);
	if (!ft_malloc_p(sizeof(t_oinst), (void **)&new_obj))
		return (report_malloc_error());
	new_obj->type = &cub->objs.firepit;
	new_obj->_id = get_new_obj_id();
	new_obj->allegiance = allegiance;
	new_obj->tex_idx = allegiance;
	obj_set_position(cub, new_obj, pos[0], pos[1]);
	new_obj->dx = pos[2];
	new_obj->dy = pos[3];
	new_obj->relative = NULL;
	new_obj->isactive = 0;
	new_obj->counter = 0;
	new_obj->action = __obj_action_firepit;
	if (link)
	{
		new_obj->relative = link;
		new_obj->isactive = 1;
	}
	new_obj->gset = new_obj->type->gsets[0];
	new_obj->next = cub->objs.instances;
	cub->objs.instances = new_obj;
	return (new_obj->_id);	
}
