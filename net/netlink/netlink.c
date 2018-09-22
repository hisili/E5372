#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/socket.h>
#include <linux/netlink.h>

#define MAX_PAYLOAD (1024)
#define NETLINK_TEST (20)
//#define NETLINK_TEST (21)

// APP => driver
// driver => APP
typedef struct _DEVICE_EVENT
{
	int device_id;
	int event_code;
	int len;
	char data[0];
} DEVICE_EVENT;

int main(int argc, char **argv)
{
	int sock_fd;
	struct sockaddr_nl src_addr, dest_addr;
	struct msghdr msg;
	struct nlmsghdr *nlh = NULL;
	DEVICE_EVENT *snd_msg_body = NULL;
	DEVICE_EVENT *rcv_msg_body = NULL;
	struct iovec iov;
	int ret;

	printf("Test NetLink\n");

	// Create a socket
	sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
	if (-1 == sock_fd) {
		printf("Error: can't create a socket, err=%d(%s)\n", errno, strerror(errno));
		return -1;
	}

	// Prepare for binding
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();		// Self-PID
	src_addr.nl_groups = 0;			// Multi-cast

	ret = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	if (ret < 0) {
		printf("Error: can't bind\n");
		close(sock_fd);
		return -1;
	}

	// Prepare for handshaking 
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	if (NULL == nlh) {
		printf("Error: malloc failed\n");
		close(sock_fd);
		return -1;
	}

	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();
	printf("my pid = %d\n", nlh->nlmsg_pid);
	nlh->nlmsg_flags = 0;

	// Real message content
	snd_msg_body = NLMSG_DATA(nlh);
	snd_msg_body->device_id = 2;
	snd_msg_body->event_code = 2;
	strcpy(snd_msg_body->data, "test-netlink");
	snd_msg_body->len = strlen(snd_msg_body->data) + 1;

	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;
	dest_addr.nl_groups = 0;

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf("Sending message ...\n");
	ret = sendmsg(sock_fd, &msg, 0);
	if (-1 == ret) {
		printf("Error: can't send message\n");
	}

	printf("Sending message: over\n");
	// Re-use the msg structure and nlh buffer
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

	// Wait for message from kernel

	while (1) {
		printf("entry loop\n");
		memset(&msg, 0, sizeof(msg));
		ret = recvmsg(sock_fd, &msg, 0);
		if (ret < 0) {
			printf("Error: can't receive message\n");
		}

		// Dispatch Message
		rcv_msg_body = NLMSG_DATA(nlh);
		printf("**** device_id = %d, event = %d, len = %d, name = %s ****\n",
				rcv_msg_body->device_id,
				rcv_msg_body->event_code,
				rcv_msg_body->len,
				rcv_msg_body->data);



		printf("exit loop\n");
	}
	
	// Close socket
	close(sock_fd);

	return 0;
}
