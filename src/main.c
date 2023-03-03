/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:07:26 by iamongeo          #+#    #+#             */
/*   Updated: 2023/03/02 20:34:36 by iamongeo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	cub_clear(t_cub *cub, int exit_status)
{
	printf("CUB CLEAR AT EXIT\n");
	if (cub->mlx)
		mlx_terminate(cub->mlx);
	return (exit_status);
}

void	on_close(void *param)
{
	t_cub	*cub;

	cub = (t_cub *)param;
	printf("CLOSE REQUEST RECEIVED !\n");
	mlx_close_window(cub->mlx);
}

void	cub_key_handler(mlx_key_data_t event, void *param)
{
//	t_cub	*cub;

//	cub = (t_cub *)param;
	if (event.key == MLX_KEY_ESCAPE && event.action == MLX_PRESS)
		on_close(param);
}

void	on_cursor_move(double xpos, double ypos, void *param)
{
	t_cub	*cub;
	double	dx;
	double	dy;

	cub = (t_cub *)param;
	(void)cub;
	dx = xpos - cub->scn_midx;
	dy = ypos - cub->scn_midy;
	cub->hero.ori += dx * ROT_FACTOR;
	printf("Cursor moved : pos (x, y) : (%lf, %lf), delta (dx, dy) : (%lf, %lf), ori : %f\n", xpos, ypos, dx, dy,
		cub->hero.ori);

	// CAN'T TOUCH THIS, naaaaah na na na tst tst na na tst tst na na.
	mlx_set_mouse_pos(cub->mlx, cub->scn_midx, cub->scn_midy);
}

// On pourra intégrer ces lignes là dans le parsing ou qqc du genre, mais on va en avoir besoin. 
int	cub_init_core_data(t_cub *cub)
{
	cub->scn_midx = SCN_WIDTH / 2;
	cub->scn_midy = SCN_HEIGHT / 2;
	cub->inv_cw = 1.0f / (float)CELL_WIDTH;
	printf("MAIN : inverse CELL_WIDTH : %.10f\n", cub->inv_cw);

	init_raycaster(cub);
	return (0);
}

int	set_player_cell_pos(t_cub *cub, int x, int y, float ori)
{
	if (get_is_wall(&cub->map, x, y))
		return (printf("ERROR hero can't be placed in wall."));
	cub->hero.cell_x = x;
	cub->hero.cell_y = y;
	cub->hero.px = x * CELL_WIDTH + (CELL_WIDTH / 2.0f);
	cub->hero.py = y * CELL_WIDTH + (CELL_WIDTH / 2.0f);
	cub->hero.ori = ori;
	return (0);
}

int	main(int argc, char **argv)
{
	t_cub		cub;
	float		*hero_cell_coord;
	
	ft_memclear(&cub, sizeof(cub));
	if (argc != 2)
		return (EXIT_FAILURE);
	cub_init_core_data(&cub);
	if (load_map(&cub, argv[1]) < 0)
		return (EXIT_FAILURE);

	if (set_player_cell_pos(&cub, 1, 5, 0.0f) != 0)
		return (EXIT_FAILURE);
	hero_cell_coord = get_grid_coords(&cub.map, cub.hero.cell_x, cub.hero.cell_y);
	printf("hero cell indexes : (%d, %d), hero cell coord : (%.3f, %.3f), hero pos : (%.2f, %.2f), hero orientation : %.5f\n",
		cub.hero.cell_x, cub.hero.cell_y, hero_cell_coord[0], hero_cell_coord[1],
		cub.hero.px, cub.hero.py, cub.hero.ori);


	if (init_raycaster(&cub) < 0)
		return (EXIT_FAILURE);

	ft_deltatime_usec_note(NULL);
	if (raycast_all_vectors(&cub) < 0)
		return (EXIT_FAILURE);

	ft_deltatime_usec_note("Raycaster results are in. What say you ?!\n");
	
	return (0);
	// FONCTION DE PARSING VIENT ICI !!
	// if (parsing_func_de_fou_debile(&cub, argc, argv) < 0)
	//	return (EXIT_FAILURE);

	printf("Try init mlx \n");
	
	// Window Width, Height, title, is_resizable. (option possible pour la window : full screen mode)
	cub.mlx = mlx_init(SCN_WIDTH, SCN_HEIGHT, "(cub)^3.D", 0);
	if (!cub.mlx)
	{
		printf("MLX init failed \n");
		return (EXIT_FAILURE);
	}
	printf("Init mlx SUCCESSFUL\n");
	mlx_focus(cub.mlx);
	mlx_focus(cub.mlx);


	// INIT DATA
	cub_init_core_data(&cub);
	
	// INIT CURSOR SETTINGS
	mlx_set_mouse_pos(cub.mlx, cub.scn_midx, cub.scn_midy);
	mlx_set_cursor_mode(cub.mlx, MLX_MOUSE_HIDDEN);

	
	// INIT HOOKS
	mlx_cursor_hook(cub.mlx, &on_cursor_move, &cub);
	mlx_key_hook(cub.mlx, &cub_key_handler, &cub);
	mlx_close_hook(cub.mlx, &on_close, &cub);
	

	printf("Starting mlx loop\n");
	mlx_loop(cub.mlx);
	printf("mlx loop stopped !\n");
	return (cub_clear(&cub, EXIT_SUCCESS));
}
