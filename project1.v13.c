/*
	Name		:	project1.c
	Version		:	12
	Milestone	:	Spanning Tree with Breadth First Search
	Author		:	George Karagiannis
	Date		:	20 Nov. 2015
	
	New in this version:
		->	
*/

#include <stdio.h>
#include <stdlib.h>

#define N 30

typedef int bool;
#define true 1
#define false 0

typedef struct edge *edge_ptr;	//pointer in "edge" structs
typedef enum {
	nt_inc,		//not_included
	incld,		//included
	chckd			//checked
} condition;
struct edge{
	//nodes
	int node_orig, node_dest;
	//extra info
	edge_ptr mirror_edge;
	/*
	*/
	
	int st_parent, st_childs, st_birth;	
	/*
		@	st_parent	:	used in both edges and nodes
				for edges	:	edge's->node_orig
		
				for nodes	:	indicate node's parent in spanning tree
								->	-1	:	no parent
								->	0	:	spanning tree's root
								->	#	:	num of parent node in st
		
		
		st_childs	:	used both in sparse array collumn's and row's arrays to indicate
						the number of node's childs in spanning tree
					->	-1	:	no parent
					->	0	:	spanning tree's root
					->	#	:	num of parent node in st
		
		st_birth	:	used in 
	*/
	
	condition st_cond;
	/*
		st_cnd	:	describes both edges' and nodes' condition on spanning tree
				->	nt_inc	:	not included in spanning tree
				->	incld	:	included in spanning tree
				->	chckd	:	included in spanning tree and checked (its childs found)
	*/
	
	edge_ptr st_parent_ptr;
	/*
	   ptr to node's parent
	   used only by nodes to constract
	   a linked list from child to parent
	*/
	
	bool cmplt_cycle;
	
	//edge's pointers
	edge_ptr 	east,	west, 	north,	south;
	/*			|		|		|		|
				right	left	up		down
	*/
	};
struct edge col_nodes[N], row_nodes[N];
//pointers in "edge" structs
edge_ptr header, temp, aux;

//functions decleration
void initialization();
void hor_insert(edge_ptr *head, edge_ptr *record);//places the edge beside the wright row
void ver_insert(edge_ptr *head, edge_ptr *record);//places the edge under the wright colunm
void create_edge(int node_orig, int node_dest, bool cr_mir);
void insert();
bool del_edge(edge_ptr *head, int node_dest);
void de1ete();
void print_list(edge_ptr *head);
void print_st(edge_ptr *head);
void print_sa(char choice);
void graph_set(int root);
void mirror_en(int node_orig, int node_dest, edge_ptr *mirror);
bool check_sets();
void span_tree();
void cycles();

int main(void){
	
	char c1='a';

    initialization();
	
	while ((c1 != 'q') && (c1 != 'Q')){
		printf("\n Your options are :\n (i)nsert\n (p)rint\n (d)elete\n (s)panning tree\n (c)ycles\n (q)uit");
		if ((c1 != 'q') || (c1 != 'Q')){
			printf("\n Give new choice: ");
            c1='a';
            fflush(stdin);
            c1=getchar();
            getchar();
            
			if ((c1 == 'q') || (c1 == 'Q')){
            	printf("\n Quit \n");
        	}
			if ((c1 == 'i') || (c1 == 'I')){
				printf("\n INSERT \n");
                insert();
        	}
        	if ((c1 == 'd') || (c1 == 'D')){
        		printf("\n DELETE \n");
                de1ete();
        	}
            if ((c1 == 'p') || (c1 == 'P')){
				printf("\n PRINT \n");
                print_sa('l');
            }
            if ((c1 == 's') || (c1 == 'S')){
            	printf("\n SPANNING TREE \n");
                span_tree();
			}
			if ((c1 == 'c') || (c1 == 'C')){
            	printf("\n CYCLES \n");
                cycles();
			}
        }
    }
	
	return 0;
}


//functions definitions
void initialization(){
	//struct edge nodes_column[30], nodes_row[30];
	int i = 0;
	
	//construction of nodes_column array
	while (i<N){
		col_nodes[i].node_orig = i+1;
    	col_nodes[i].node_dest = 0;
    	col_nodes[i].mirror_edge = row_nodes+i;
    	col_nodes[i].st_parent = -1;
    	col_nodes[i].st_childs = 0;
    	col_nodes[i].st_birth = -1;
    	col_nodes[i].st_cond = nt_inc;
    	col_nodes[i].st_parent_ptr = NULL;
    	col_nodes[i].cmplt_cycle = false;
    	col_nodes[i].north = NULL;
    	col_nodes[i].south = NULL;
    	
    	if (i==N-1){
    		col_nodes[i].east = NULL;
    		col_nodes[i].west = &col_nodes[i-1];
		}
    	else if (i==0){
    		col_nodes[i].east = &col_nodes[i+1];
    		col_nodes[i].west = NULL;
		}
		else /*(i>0 && i<N-1)*/{
			col_nodes[i].east = &col_nodes[i+1];
    		col_nodes[i].west = &col_nodes[i-1];
		}
		i++;
	}
	
	//construction of nodes_row array
	i = 0;
	while (i<N){
		row_nodes[i].node_orig = 0;
    	row_nodes[i].node_dest = i+1;
    	row_nodes[i].mirror_edge = col_nodes+i;
    	row_nodes[i].st_parent = -1;
    	row_nodes[i].st_childs = 0;
    	row_nodes[i].st_birth = -1;
    	row_nodes[i].st_cond = nt_inc;
    	row_nodes[i].st_parent_ptr = NULL;
    	row_nodes[i].cmplt_cycle = false;
    	row_nodes[i].east = NULL;
    	row_nodes[i].west = NULL;
    		
    	if (i==N-1){
    		row_nodes[i].north = &row_nodes[i-1];
    		row_nodes[i].south = NULL;
		}
    	else if (i==0){
    		row_nodes[i].north = NULL;
    		row_nodes[i].south = &row_nodes[i+1];
		}
		else /*(i>1 && i<30)*/{
    		row_nodes[i].north = &row_nodes[i-1];
    		row_nodes[i].south = &row_nodes[i+1];
		}
		i++;
	}
}


//----------------------------------------------------------------------------------------------------------//
//											   GRAPH FUNCTIONS  											//
//----------------------------------------------------------------------------------------------------------//
void hor_insert(edge_ptr *head, edge_ptr *record){
	
	edge_ptr list_head = (*head)->east;
	bool empty, min, only_one;
	/*
	empty	->	true	:	no edges in this column
			->	false	:	there is(are) edge(s) in this column
	
	only_one	->	true	:	if column's LIST has only one element
				->	false	:	if column's LIST has NOT only one element
	
	min		->	true	:	if new node_dest is smaller than FIRST edge's node_dest
			->	false	:	if new node_dest is NOT smaller than FIRST edge's node_dest
	*/
	
	//flag initialization
	empty = (*head)->east == NULL ? true : false;
	if (!empty){
		only_one = list_head->east == NULL ? true : false;
		min = list_head->node_orig > (*record)->node_orig ? true : false;
	}
	
	if ((!empty) && (min)){
		(*record)->east = list_head;
		(*record)->west = list_head->west;
		list_head->west->east = *record;
		list_head->west = *record;
	}
	if ((!empty) && (!min) && (!only_one)){
		aux = list_head;//head->south;
		while((aux->east != NULL) && ((*record)->node_orig > aux->east->node_orig)){
			aux = aux->east;
		}
		(*record)->east = aux->east;
		(*record)->west = aux;
		//check if aux points at LAST record
		if (aux->east != NULL){
			aux->east->west = *record;
		}
		aux->east = *record;
	}
	if ((!empty) && (!min) && (only_one)){
		list_head->east = *record;
		(*record)->west = list_head;
	}
	if (empty){
		(*head)->east = *record;
		(*record)->west = *head;
	}
}


void ver_insert(edge_ptr *head, edge_ptr *record){
	
	edge_ptr list_head = (*head)->south;
	bool empty, min, only_one;
	/*
	empty	->	true	:	no edges in this column
			->	false	:	there is(are) edge(s) in this column
	
	only_one	->	true	:	if column's LIST has only one element
				->	false	:	if column's LIST has NOT only one element
	
	min		->	true	:	if new node_dest is smaller than FIRST edge's node_dest
			->	false	:	if new node_dest is NOT smaller than FIRST edge's node_dest
	*/
	
	//flag initialization
	empty = (*head)->south == NULL ? true : false;
	if (!empty){
		only_one = list_head->south == NULL ? true : false;
		min = list_head->node_dest > (*record)->node_dest ? true : false;
	}
	
	if ((!empty) && (min)){
		(*record)->south = list_head;
		(*record)->north = list_head->north;
		list_head->north->south = *record;
		list_head->north = *record;
	}
	if ((!empty) && (!min) && (!only_one)){
		aux = list_head;
		while((aux->south != NULL) && ((*record)->node_dest > aux->south->node_dest)){
			aux = aux->south;
		}
		(*record)->south = aux->south;
		(*record)->north = aux;
		//check if aux points at LAST record
		if (aux->south != NULL){
			aux->south->north = *record;
		}
		aux->south = *record;
	}
	if ((!empty) && (!min) && (only_one)){
		list_head->south = *record;
		(*record)->north = list_head;
	}
	if (empty){
		(*head)->south = *record;
		(*record)->north = *head;
	}
}


void create_edge(int node_orig, int node_dest, bool cr_mir){
	
	bool empty;
	
	//construct a temporary struct of edge   
	temp=(edge_ptr)malloc(sizeof(struct edge));
	temp->node_orig = node_orig;
	temp->node_dest = node_dest;
	temp->mirror_edge = NULL;
	temp->st_parent = -1;
	temp->st_childs = 0;
	temp->st_birth = -1;
	temp->st_cond = nt_inc;
	temp->st_parent_ptr = NULL;
	temp->cmplt_cycle = false;
	temp->east = NULL;
	temp->west = NULL;
	temp->north = NULL;
	temp->south = NULL;
	
	header = col_nodes+node_orig-1;
	ver_insert(&header, &temp);
	header = row_nodes+node_dest-1;
	hor_insert(&header, &temp);
	
	if (cr_mir){
		create_edge(node_dest, node_orig, false);
	}
	
	//find mirror edge
	if (temp->mirror_edge!=NULL){
		aux = col_nodes + node_dest - 1;
		empty = aux->south == NULL ? true : false;
		
		if (!empty){
			aux = aux->south;
			while (aux->node_dest != node_orig){
				aux = aux->south;
			}
		}
	
		temp->mirror_edge = aux;
		aux->mirror_edge = temp;
	}
}


void cnct_mirrors(node_1, node_2){
	
	edge_ptr aux1, aux2;
	
	aux1 = col_nodes + node_1 - 1;
	aux2 = col_nodes + node_2 - 1;
	
	while (aux1->node_dest != node_2){
		aux1 = aux1->south;
	}
	
	while (aux2->node_dest != node_1){
		aux2 = aux2->south;
	}
	
	aux1->mirror_edge = aux2;
	aux2->mirror_edge = aux1;
}


void insert(){
	
	int node_orig, node_dest;
	bool need_mirror;
	/*
	need_mirror	->	true	:	creates mirror edge
				->	false	:	do not create mirror edge, edge is in trace
	
	in case of no need to create the mirror, please comment
	only the if clause which checks that flag [if (need_mirror)]
	*/
	
	printf("\n Give me the origin node...\t\t:\t");
	scanf("%d", &node_orig);
    getchar();
    printf(" and the destination node, please!\t:\t");
    scanf("%d", &node_dest);
    getchar();
    
    //check if needs to create a mirror edge
	need_mirror = node_orig==node_dest ? false : true;
    
	create_edge(node_orig, node_dest, need_mirror);
	
	if (need_mirror){
		cnct_mirrors(node_orig, node_dest);
	}
}


bool del_edge(edge_ptr *head, int node_dest){
	
	int node_orig;
	edge_ptr next, list_head = (*head)->south;
	bool empty, only_one, equal, del = false;
	/*
	empty	->	true	:	no edges in this column
			->	false	:	there is(are) edge(s) in this column
	
	only_one	->	true	:	if column's LIST has only one element
				->	false	:	if column's LIST has NOT only one element
	
	equal		->	true	:	if new node_dest is equal to FIRST edge's node_dest
				->	false	:	if new node_dest is NOT equal to FIRST edge's node_dest
	*/
	
	//flag initialization
	empty = list_head == NULL ? true : false;
	if (!empty){
		only_one = list_head->south == NULL ? true : false;
		equal = list_head->node_dest == node_dest ? true : false;
	}
	
	//serch in column for edge 
	if (empty){
		printf("\n Sorry! No edge starts from node %d", (*head)->node_orig);
	}
    if ((!empty) && (only_one) && (!equal)){
    	printf("\n Sorry! No edge ends at node %d", node_dest);
	}    
    if ((!empty) && (only_one) && (equal)){
    	temp = list_head;
    	list_head = temp->south;
    	del = true;
    	printf("\n It was the only edge starting from this node.\n Deleting...");
	}
	if ((!empty) && (!only_one) && (!equal)){
		aux = list_head;
		
		while ((aux->south != NULL) && (aux->south->node_dest != node_dest)){
			aux = aux->south;
		}
		
		if (aux->south == NULL){
			printf(" Sorry! No edge ends at node %d\n", node_dest);
		}        
        if ((aux->south != NULL) && (aux->south->south == NULL)){
			temp = aux->south;
			aux->south = temp->south;
			del = true;
			printf("\n It was the last edge starting from this node.\n Deleting...");
		}
        if ((aux->south != NULL) && (aux->south->south != NULL)){
        	temp = aux->south;
        	//next = aux->south->south
        	aux->south = aux->south->south;//next;
        	temp->south->north = aux;//next->north -> aux;
        	del = true;
        	printf("\n Edge found. Deleting...");
		}
	}
	if ((!empty) && (!only_one) && (equal)){
		temp = list_head;
		list_head = temp->south;
		temp->south->north = temp->north;
		del = true;
		printf("\n It was the first edge starting from this node.\n Deleting...");
	}
	
	//relocate the east/west ptrs and free memory
	if (del){
		node_orig = temp->node_orig;
		if (temp->east != NULL){
			temp->east->west = temp->west;
		}
		temp->west->east = temp->east;
		free(temp);
		printf("\n Edge from node %d to node %d deleted!\n", node_orig, node_dest);
		return true;
	}
	
	return false;
}


void de1ete(){
	
	int node_orig, node_dest;
	bool need_mirror, del_done;
	/*
	del_done	->	true	:	edge from node_orig to node_dest found and deleted
				->	false	:	edge from node_orig to node_dest NOT found, so there isn't any mirror edge
				
	need_mirror	->	true	:	there is mirror edge to delete
				->	false	:	there is NOT mirror edge to delete, edge is in trace
	
	in case of no need to delete any mirror edge, please comment
	only the if clause which checks that flag [if (need_mirror)]
	*/
	
	printf("\n Give me the origin node...\t\t:\t");
	scanf("%d", &node_orig);
    getchar();
    printf(" and the destination node, please!\t:\t");
    scanf("%d", &node_dest);
    getchar();
    
    //check if needs to delete a mirror edge
	need_mirror = node_orig==node_dest ? false : true;
	
	printf("\n Deleting edge from node %d to node %d", node_orig, node_dest);
	header = col_nodes+node_orig-1;
	del_done = del_edge(&header, node_dest);
	
	if (need_mirror && del_done){
		printf("\n Deleting mirror edge from node %d to node %d", node_dest, node_orig);
		header = col_nodes+node_dest-1;
		del_done = del_edge(&header, node_orig);
	}
	printf("\n");
}


void print_list(edge_ptr *head){
	
	int tabs;
	aux = (*head)->east;
	
	if(aux!=NULL){
		tabs = aux->node_orig - aux->west->node_orig;
		while(tabs!=0){
			printf("\t");
			tabs--;
		}
		printf("(%d,%d)", aux->node_orig, aux->node_dest);
		print_list(&aux);
	}
}


void print_st(edge_ptr *head){
	
	edge_ptr aux2;
	int tabs;
	aux = (*head)->east;
	
	if(aux!=NULL){
		if (aux->st_cond != nt_inc){
		//if (aux->st_included){
			aux2 = aux->west;
			while(aux2->st_cond == nt_inc){
			//while(!aux2->st_included){
				aux2 = aux2->west;
			}
			tabs = aux->node_orig - aux2->node_orig;
			while(tabs!=0){
				printf("\t");
				tabs--;
			}
			printf("(%d,%d)", aux->node_orig, aux->node_dest);
		}
		print_st(&aux);
	}
}


void print_sa(char choice){
	
	int row = 1, col = 1;
	
	printf("\n COLUMNS\t->\tORIGIN NODES\n ROWS\t\t->\tDESTINATION NODES\n");
	printf(" Every node in sparse array appears as pair (origin,destination)\n\n");
	while(col<=N){
		printf("\t  %d", col);
		col++;
	}
	printf("\n\n");
	
	while(row<=N){
		header = row_nodes+row-1;
		if (row<10){
			printf(" ");
		}
		printf("%d|", row);
		if (choice=='l'){
			print_list(&header);
		}
		else if (choice=='s'){
			print_st(&header);
		}
		printf("\n");
		row++;
	}
	printf("\n");
}


//----------------------------------------------------------------------------------------------------------//
//											SPANNING TREE FUNCTIONS											//
//----------------------------------------------------------------------------------------------------------//
void graph_set(int root){
	/*
		scans col_nodes array checking which of them
		are included in graph to implement graph's set
		and marks spanning tree's root
	*/
	
	aux = col_nodes;
	while (aux != NULL){
		
		if (aux->south != NULL){
			aux->st_cond = incld;
		}
		
		//	set spanning tree's root
		if (aux->node_orig == root){
			aux->st_parent = 0;
			aux->st_birth = 0;
		}
		
		aux = aux->east;
	}
}


bool compare_sets(){
	
	/*
		compares col_nodes (graph set) and row_nodes (st set)
		to check if all their elements have the same st_cond
		returns true if all are same, false otherwise
	*/
	edge_ptr col_aux = col_nodes, row_aux = row_nodes;
	
	while ((col_aux != NULL) && (row_aux != NULL)){
		if (col_aux->st_cond != row_aux->st_cond){
			return false;
		}
		col_aux = col_aux->east;
		row_aux = row_aux->south;
	}
	return true;
}


void span_tree(){
	
	/*
		traverses graph starting from root and marks each node's childs
		repeats until all nodes of graph included in spanning tree
	*/
	edge_ptr p_aux, c_aux, m_aux, st_set_aux, g_set_aux;
	/*
		p_aux		:	p(arent) aux pointer moves along col_nodes[] (i.e. graph's set)
		c_aux		:	c(hild) aux pointer moves along list of edge structs starting from p_aux
		st_set_aux	:	s(panning)t(ree) aux pointer moves along row_nodes[] (i.e. spanning tree's set)
		g_set_aux	:	g(raph) aux pointer moves along col_nodes[] (i.e. graph's set)
	*/
	int root, parent = 0 , childs, birth = 0;
	bool same = false;
	/*
		consider col_nodes & row_nodes as two sets where	:	
						col_nodes	=	set of nodes included in graph
						row_nodes	=	set of nodes included in spanning tree
		then
				same	->	true	:	sets are equal
						->	false	:	sets are NOT equal
	*/
	printf(" Select the root node\t:\t");
	scanf("%d", &root);
    getchar();
	/*
		scan col_nodes array checking which of them
		are included in graph to implement graph's set
		and mark spanning tree's root
	*/
	graph_set(root);
	
	
	//start graph traversal
	p_aux = col_nodes;
	while (!same){
		
		//search for next valid node to add spanning tree
		while ((p_aux->st_birth != birth) || (p_aux->st_cond == chckd)){
			
			p_aux = p_aux->east;
			
			if (p_aux == NULL){
				p_aux = col_nodes;
				birth++;
			}
		}
		printf("\n I found node\t:\t%d\t who belongs in generation\t:\t%d\tof spanning tree", p_aux->node_orig, p_aux->st_birth);
		
		//find childs of parent p_aux pointing
		printf("\n Searching its childs...");
		childs = 0;
		c_aux = p_aux->south;
		while(c_aux != NULL){
			
			st_set_aux = row_nodes + c_aux->node_dest - 1;
			
			if ((c_aux->st_cond != chckd) && (st_set_aux->st_cond == nt_inc)){
				printf("\n Node\t:\t%d", c_aux->node_dest);
			/*
				!c_aux->st_checked			:	edge has NOT been checked	
				!st_set_aux->st_included	:	node has NOT included in ST set 
			*/
				//mark edge's st_variables on sparse array
				c_aux->st_parent = c_aux->node_orig;
				c_aux->st_childs = c_aux->node_dest;
				c_aux->st_birth = birth + 1;
				c_aux->st_cond = chckd;
				//mark mirror edge's st_variables on sparse array
				m_aux = c_aux->mirror_edge;
				m_aux->st_parent = c_aux->st_parent;
				m_aux->st_childs = c_aux->st_childs;
				m_aux->st_birth = c_aux->st_birth;
				m_aux->st_cond = c_aux->st_cond;
				//mark child-node's st_variables on col_nodes[]
				g_set_aux = col_nodes + c_aux->node_dest - 1;
				g_set_aux->st_parent = c_aux->node_orig;
				g_set_aux->st_birth = c_aux->st_birth;
				g_set_aux->st_parent_ptr = col_nodes + c_aux->st_parent - 1;
				//mark child-node's st_variables on row_nodes[]
				st_set_aux->st_parent = c_aux->node_orig;
				st_set_aux->st_birth = c_aux->st_birth;
				st_set_aux->st_cond = incld;
				st_set_aux->st_parent_ptr = row_nodes + c_aux->st_parent - 1;
				childs++;
				printf("\tis child # %d\tof parent-node\t%d\t and belongs in generation # %d\t", childs, c_aux->st_parent, c_aux->st_birth);
			}
			c_aux = c_aux->south;
		}
		
		//after all parent-node's childs found
		p_aux->st_childs = childs;
		p_aux->st_cond = chckd;
		//move st_set_aux ptr on last checked node
		st_set_aux = row_nodes + p_aux->node_orig - 1;
		st_set_aux->st_parent = p_aux->st_parent;
		st_set_aux->st_childs = p_aux->st_childs;
		st_set_aux->st_birth = p_aux->st_birth;
		st_set_aux->st_cond = chckd;
		printf("\n Finally, node # %d\t in generation %d\t has %d childs\n", p_aux->node_orig, p_aux->st_birth, p_aux->st_childs);
				
		//compare st & graph's sets if they're equal
		same = compare_sets();
	}
	
	printf("\n\n\nSpanning Tree Found");
	print_sa('s');
}


//----------------------------------------------------------------------------------------------------------//
//											  CYCLES FUNCTIONS  											//
//----------------------------------------------------------------------------------------------------------//
void cycles(){
	
	/*
		checks each edge's st_cond and search the cycle for those
		which are includes in graph but not in spanning tree
	*/
	edge_ptr n_aux = col_nodes, e_aux, lst1_aux, lst2_aux;
	
	while (n_aux!=NULL){
		e_aux = n_aux->south;
		while (e_aux!=NULL){
			if ((e_aux->st_cond != chckd) && (!e_aux->cmplt_cycle)){
				printf("\n\n Edge from node %d to node %d, completes cycle of nodes:\n", e_aux->node_orig, e_aux->node_dest);
				//find edge's nodes
				lst1_aux = col_nodes + e_aux->node_orig - 1;
				lst2_aux = col_nodes + e_aux->node_dest - 1;
				
				while (lst1_aux->st_birth != lst2_aux->st_birth){
					if (lst1_aux->st_birth > lst2_aux->st_birth){
						printf("\t%d", lst1_aux->node_orig);
						lst1_aux = lst1_aux->st_parent_ptr;
					}
					else if (lst1_aux->st_birth < lst2_aux->st_birth){
						printf("\t%d", lst2_aux->node_orig);
						lst2_aux = lst2_aux->st_parent_ptr;
					}
				}
				
				while (lst1_aux != lst2_aux){
					printf("\t%d", lst1_aux->node_orig);
					lst1_aux = lst1_aux->st_parent_ptr;
					printf("\t%d", lst2_aux->node_orig);
					lst2_aux = lst2_aux->st_parent_ptr;
				}
				
				printf("\t%d", lst1_aux->node_orig);
			}
			e_aux->cmplt_cycle = true;
			e_aux->mirror_edge->cmplt_cycle = true;
			e_aux = e_aux->south;
		}
		
		n_aux = n_aux->east;
	}
	printf("\n");
}


