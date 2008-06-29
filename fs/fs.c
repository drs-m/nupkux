/*
 *  Copyright (C) 2007,2008 Sven Köhler
 *
 *  This file is part of Nupkux.
 *
 *  Nupkux is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nupkux is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Nupkux.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fs/fs.h>
#include <mm.h>
#include <errno.h>

fs_node *fs_root = 0;
mountinfo *mountinfos = 0;

void open_fs(fs_node *node, UCHAR read, UCHAR write)
{
	if (node->f_op && node->f_op->open) return node->f_op->open(node);
}

UINT read_fs(fs_node *node, UINT offset, UINT size, UCHAR *buffer)
{
	if (node->f_op && node->f_op->read) return node->f_op->read(node,offset,size,buffer);
		else return 0;
}

UINT write_fs(fs_node *node, UINT offset, UINT size, UCHAR *buffer)
{
	if (node->f_op && node->f_op->write) return node->f_op->write(node,offset,size,buffer);
		else return 0;
}

void close_fs(fs_node *node)
{
	if (node->f_op && node->f_op->close) return node->f_op->close(node);
}

struct dirent *readdir_fs(fs_node *node, UINT index)
{
	if ((node->flags&FS_DIRECTORY) && (node->f_op && node->f_op->readdir)) return node->f_op->readdir(node,index);
		else return 0;
}

fs_node *finddir_fs(fs_node *node, char *name)
{
	if ((node->flags&FS_DIRECTORY) && (node->f_op && node->f_op->finddir)) return node->f_op->finddir(node,name);
		else return 0;
}

mountinfo *fs_add_mountpoint(UINT fs_type, void *discr, fs_node *mountpoint, fs_node *device, vfs_nodes *nodes)
{
	mountinfo *mi = malloc(sizeof(mountinfo));
	
	mi->fs_type=fs_type;
	mi->discr=discr;
	mi->mountpoint=mountpoint;
	mi->device=device;
	mi->next=mountinfos;
	mi->nodes=nodes;
	mountinfos=mi;
	if (mountpoint) {
		//FIXME I could mount another time on this node and make a lot of trouble
		if (mountpoint->flags!=FS_DIRECTORY) {
			errno=-ENOTDIR;
			return 0;
		}
		mountpoint->flags|=FS_MOUNTPOINT;
		mountpoint->ptr=nodes->root;
	}

	return mi;
}

UINT fs_del_mountpoint(mountinfo *mi)
{
	mountinfo *pre=0,*tmp=mountinfos;
	
	while (tmp) {
		if (tmp==mi) break;
		pre=tmp;
		tmp=tmp->next;
	}
	if (!tmp) return 0;
	if (!pre) mountinfos=tmp->next;
		else pre->next=tmp->next;
	if (mi->mountpoint) {
		mi->mountpoint->flags&=~FS_MOUNTPOINT;
		mi->mountpoint->ptr=0;
	}
	return 1;
}
