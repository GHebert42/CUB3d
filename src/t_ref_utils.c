/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   t_ref_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gehebert <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/24 08:22:57 by gehebert          #+#    #+#             */
/*   Updated: 2023/04/24 08:23:01 by gehebert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

/*transfere ref "A a,b,c,d" into xwalls[4]*/ 
char *get_ref_str(t_cub *cub, char *ref, int alt)
{
    //find txtr ref 
    char *tex_str;
    int x = 0;

    while(*cub->mx->full[x])
    {
        if (*cub->mx->full[x] != ref[alt + 2])
            x++;
        else 
            break;
    }
    // printf("::: SPACEX --id(%d) -- ref[%c] -- name{%s}\n", alt, ref[alt+2], cub->mx->full[x]);    
    tex_str = cub->mx->full[x];
    return (tex_str);       
}

t_cub	*get_tex_by_ref(t_cub *cub, int id, const char *tex_str)
{
	char	*t;
    const char    *aux;
    int     alt;

    alt = 0;
    while (alt <= 3 && cub->mx->xnum > cub->tex_id)
    {
        if (!cub->tex.walls[cub->tex_id] )
        {
            aux =  get_ref_str(cub, (char *)tex_str, alt);
            t = ft_substr(aux, 2, ft_strlen(aux)-2);
            cub->tex.walls[cub->tex_id] = mlx_load_png(t);
            if (!cub->tex.walls[cub->tex_id])
                return (report_mlx_tex_load_failed(t));
            cub->tex_id++;
            printf(" txtr_id(%d) REF(%c) TEX_PATH__%s\n", cub->tex_id, tex_str[alt+2], t);
            alt++;
        }
        else
        {
            ft_eprintf("Error\n\t- Trying to load texture_BY_Ref id %d twice.\n", id);
            return (NULL);
        }	
    }
	return (cub);
}

char *t_name_set(const char *dir_path, char *d_name)
{
    char *full_name;

    full_name = ft_strjoin(dir_path, d_name);
    return (full_name);
}

const char *get_folder_name(char *full_path)
{
    char *folder_name;
    int strt;
    int end;

    strt = 0;
    end = 0;
    while(full_path[strt] != '\0')
    {
        if (full_path[strt] == '.')
        {
            end = strt;
            break;
        }
        ++strt;
    }
    while(*full_path)
    {
        if (full_path[strt] != 47)
            --strt;
        else if (full_path[strt] == 47)
            break;
    }
    folder_name = ft_substr(full_path, strt, (end - strt));
    folder_name = ft_strjoin ("tex", folder_name);
    folder_name = ft_strjoin (folder_name, "/");
    printf("GET_FLD_NAME:: %s\n",folder_name); 
    return ((const char *)folder_name);
}
/* go get xwalls now : where */
    /*  
        get     : str* to deploy  == tex_str ***
                :   {"A  a,b,c,d"} == xwalls[4] 
                :
                : set each lower case to a path to replace tex_str
                :   alt value = array pos;

        set     : assign  lower_char ref with full_path to tex_str
                :   array pos char ++;

        ex: alt = 0 , get {"a tex/...png"}    */