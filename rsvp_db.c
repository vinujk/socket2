#include "rsvp_db.h"
#include "timer-event.h"

struct session* sess = NULL;
struct session* head = NULL;
time_t now = 0;

	
struct session* insert_session(struct session* sess, char sender[], char receiver[], u_int8_t dest) {
        now = time(NULL);
        printf("insert session\n");
        if(sess == NULL) {
                struct session *temp = (struct session*)malloc(sizeof(struct session));
                if(temp < 0)
                         printf("cannot allocate dynamic memory]n");

                temp->last_path_time = now;
                strcpy(temp->sender, sender);
                strcpy(temp->receiver, receiver);
		temp->dest = dest;
                temp->next = NULL;
                return temp;
        } else {
		struct session *local = NULL;
                while(sess != NULL) {
                        if((strcmp(sess->sender, sender) == 0) &&
                           (strcmp(sess->receiver, receiver) == 0)) {
				sess->last_path_time = now;
                                return;
                        }
			local = sess;
                        sess=sess->next;
                }

                struct session *temp = (struct session*)malloc(sizeof(struct session));
                if(sess < 0)
                         printf("cannot allocate dynamic memory\n");

                temp->last_path_time = now;
		strcpy(temp->sender, sender);
                strcpy(temp->receiver, receiver);
		temp->dest = dest;
                temp->next = NULL;

                local->next = temp;
        }
}


struct session* delete_session(struct session* sess, char sender[], char receiver[]) { 

        struct session *temp = NULL;
	struct session *head = sess;

        printf("delete session\n");
        while(sess != NULL) {
                if((head == sess) &&
                   (strcmp(sess->sender, sender) == 0) &&
                   (strcmp(sess->receiver, receiver) == 0)) {
                        temp = head;
                        head = head->next;
                        free(temp);
                        return head;
                } else {
                        if((strcmp(sess->sender, sender) == 0) &&
                           (strcmp(sess->receiver, receiver) == 0)) {
				temp = sess->next;
                                *sess = *sess->next;
                                free(temp);
                        }else{
                                sess = sess->next;
                        }
                }
        }
}

