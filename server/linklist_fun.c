#include "server.h" 
/*在表头插入*/
void user_insert(struct USER *inse_user)
{
//	struct USER *current = inse_user;     
	struct USER *new_user = (struct USER*)malloc(sizeof(struct USER));
//	memset(new_user, 0, sizeof(struct USER));
		
	
	if (new_user == NULL)
	{
		fprintf(stderr,"insert malloc error!\n");
	}
	else
	{
		fprintf(stdout, "insert malloc succeed!\n");
	}
	
    *new_user = *inse_user;                 /*重要!!!注意不要new_user = inse_user!!!!*/
    new_user->next = NULL;
    
    fprintf(stdout, "\n\nnew_suer->user_ID = %ld\nnew_user->name = %s\nnew_user->secret = %s\n\n",
                    new_user->user_ID, new_user->name, new_user->secret);
    
	new_user->next = first->next;
	first->next = new_user;
	user_output();
}

/*寻找ID,然后删除*/
void user_remove(struct USER *rm_user)
{
	struct USER *del = NULL;
	struct USER *current = NULL; 
	struct USER cp_user;
	
	
	
	cp_user = *rm_user;
	for (current = first; current->next != NULL; current = current->next)
	{
		del = current->next;

		if (del->user_ID == cp_user.user_ID)		/*del指向要删除的用户*/
		{
		    
			current->next = del->next;
			free(del);
			break;
		}
	}
	printf("删除元素后的输出:\n");
	user_output();
}


void user_output()
{
    struct USER *current = first->next;
    printf("现在输出链表的信息:\n");
    while(current != NULL)
    {
        fprintf(stdout, "current->ID = %ld\n", current->user_ID);
        fprintf(stdout, "current->name = %s\n", current->name);
        fprintf(stdout, "current->secret = %s\n\n", current->secret);
        
        current = current->next;
    }
}
