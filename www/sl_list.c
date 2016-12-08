#include "sl_list.h"

struct s_list_head *new_s_list_head()
{
	struct s_list_head *nh =
		(struct s_list_head *)malloc(sizeof(struct s_list_head));
	if(nh)
	{
		nh->first = nh->last = 0;
	}
	return nh;
}

struct s_list_node *new_slist_node(void *data)
{
	struct s_list_node *nn =
			(struct s_list_node*)malloc(sizeof(struct s_list_node));
	if(nn)
	{
		nn->data = data;
		nn->next = 0;
	}
	return nn;
}

void s_list_add(struct s_list_head *lst, void *data)
{
	struct s_list_node *nn = new_slist_node(data);

	if(!lst->first)
	{
		lst->first = lst->last = nn;
		return;
	}
	lst->last = lst->last->next = nn;
}

