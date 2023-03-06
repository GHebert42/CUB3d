/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gehebert <gehebert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:33:38 by iamongeo          #+#    #+#             */
/*   Updated: 2023/03/06 18:19:04 by gehebert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CUB3D_H__
# define __CUB3D_H__

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <string.h>
# include <math.h>

# include "MLX42/MLX42.h"
# include "libft.h"
# include "mtxlib.h"

# define SCN_WIDTH  800
# define SCN_HEIGHT 640
//# define ROT_FACTOR 0.012271846f // (2*PI / 512), soit 1 512e de tour pour chaque deltaX de souris (Temporaire, à teester)
# define ROT_FACTOR 0.006135923f
# define CELL_WIDTH 64
# define M_TAU 6.283185307179586f// 2 * pi

# define FOV120 2.0943951023931953f// 120 degrees : 2.0943951023931953f, 90 degrees : 1.5707963267948966f
# define FOV120_HF 1.0471975511965976f// 120 degrees : 2.0943951023931953f, 90 degrees : 1.5707963267948966f

# define FOV90 1.5707963267948966f
# define FOV90_HF 0.7853981633974483f

# define FOV60 1.047197551196597746f
# define FOV60_HF 0.52359877559829887f

# define FOV FOV60//	2.0943951023931953f// 120 degrees : 2.0943951023931953f, 90 degrees : 1.5707963267948966f
# define FOV_HF FOV60_HF//	1.0471975511965976f// 120 degrees : 2.0943951023931953f, 90 degrees : 1.5707963267948966f

# define ENABLE_MINIMAP 1

# define MINIMAP_WIDTH 200
# define MINIMAP_HEIGHT 200
# define MINIMAP_PADX 20
# define MINIMAP_PADY 20

enum	e_sides
{
	W_SIDE = 0,
	N_SIDE = 1,
	E_SIDE = 2,
	S_SIDE = 3
};

typedef struct s_map_data
{
	int	width;
	int	height;
	int	total_cells;
	int	width_px;
	int	height_px;
	int	total_area;
	char	**tab;		// 2D array (could be 1D, char * instead)
	char	*collision_map;	// 1D array 
	float	**grid_coords;//	mtx indexed (y * width + x) rowwise and column 0 and 1 represant the x and y coordinates in world coords.
}	t_map;

// All 4 elem arrays of textures organized as W, N, E, S, according to the side they represent.
// Potentially, animated wall textures could be in a 2D array[side][anim_iteration] up to n nb of frames in the animation.
typedef struct s_texture_data
{
	mlx_texture_t	*walls[4];	// pointers returned from mlx_load_png(path)
	mlx_texture_t	*skymap;	// yessss
}	t_tex;

typedef struct s_main_character_data
{
	int		cell_x;
	int		cell_y;
	float	px;
	float	py;
	float	ori;
	t_mtx	*theta_offsets;	// linspace from -half_fov to +half_fov. DOES NOT take into account player orientation
	t_mtx	*ray_thetas;	// theta_offsets + player ori. Specific thetas for player pos. Call update_rays().
	t_mtx	*rays[2];	// first ptr is the cosine array from linspace thetas, second is sin array from thetas.
	t_mtx	*fisheye_correctors;// = mtx_cos(theta_offsets); multiplied to collision dist to correct fisheye lens.
	t_mtx	*coll_walls;	// cell coords of the wall that was hit for all rays.
	t_mtx	*coll_sides;	// side that was hit checkable as e_side enum for all rays.
	t_mtx	*collisions;	// intersections with walls in x y world coords;
	t_mtx	*distances;	// 1D vect, len nb of rays, with distances to collisions
	t_mtx	*tex_infos;	// 2D vect, len nb of rays;
				// row[0] = ratio of wall hit position to CELL_WIDTH from
				// 	the left to the right side of the wall. Multiply this ratio to texture width to 
				// 	get exact column to draw in texture buffer.
				// row[1] = height of column to draw on screen. Divide texture height by this nb
				// 	to get the increment of pixels to travel in texture buffer column
				//	for each screen pixel to draw on screen. Draw on screen buffer from y = SCN_MIDY -
				//	0.5 * row[1] to SCN_MIDY + 0.5 * row[1] (and x is the index of the ray being
				//	rendered.)
				// 

	float	*dirx;	// ptr to rays[0][SCN_WIDTH / 2], the x part of the player's directional vector.
	float	*diry;	// ptr to rays[1][SCN_WIDTH / 2], the y part of the player's directional vector.
}	t_hero;

typedef struct s_renderer
{
	mlx_image_t	*bg_layer;
	mlx_image_t	*walls_layer;
	mlx_image_t	*ui_layer;

	int		requires_update;
}	t_rdr;

typedef struct s_cub3d_core_data
{
	mlx_t	*mlx;

	int	scn_midx;	// mid screen x coordinate
	int	scn_midy;	// mid screen y coordinate
	float	inv_cw;		// inverse CELL_WIDTH. precalc const division for optimisation

	float	fov;// = fov;// field of view
	float	hfov;// = fov * 0.5f;// half fov
	float	near_z;// = (0.5f * (float)SCN_WIDTH) / tanf(cub->hfov);
	float	near_proj_factor;// = CELL_WIDTH * cub->near_z;

	t_map	map;
	t_tex	tex;
	t_hero	hero;
	t_rdr	renderer;
}	t_cub;


/// PARSING ///////////////////
int	load_map(t_cub *cub, char *map_file);

/// RAYCASTER /////////////////
int	init_raycaster(t_cub *cub);
int	raycast_all_vectors(t_cub *cub);
void	update_rays(t_cub *cub);
void	update_fov(t_cub *cub, float fov);
char	get_is_wall(t_map *map, int cx, int cy);
float	*get_grid_coords(t_map *map, int cx, int cy);

/// DDA ALGO //////////////////

/// RENDERER /////////////////
int	renderer_clear(t_cub *cub);
//void	render_walls(t_cub *cub);
void	render_scene(t_cub *cub);
int	init_renderer(t_cub *cub);

/// CHARACTER CONTROLS ////////
void	cub_player_rotate(t_cub *cub, float rot);
void	cub_player_move(t_cub *cub, float d_walk, float d_strafe);
void	cub_player_zoom(t_cub *cub, float dz);


/// ERROR HANDLING ////////////
int	report_mlx_init_error(void);

#endif