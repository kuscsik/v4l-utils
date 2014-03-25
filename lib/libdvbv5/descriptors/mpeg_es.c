/*
 * Copyright (c) 2013 - Andre Roth <neolynx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#include "descriptors/mpeg_es.h"
#include "descriptors.h"
#include "dvb-fe.h"

int dvb_mpeg_es_seq_start_init(const uint8_t *buf, ssize_t buflen, struct dvb_mpeg_es_seq_start *seq_start)
{
	if(buflen < sizeof(struct dvb_mpeg_es_seq_start))
		return -1;
	memcpy(seq_start, buf, sizeof(struct dvb_mpeg_es_seq_start));
	bswap32(seq_start->bitfield);
	bswap32(seq_start->bitfield2);
	bswap32(seq_start->bitfield3);
	return 0;
}

void dvb_mpeg_es_seq_start_print(struct dvb_v5_fe_parms *parms, struct dvb_mpeg_es_seq_start *seq_start)
{
	dvb_log("MPEG ES SEQ START");
        dvb_log(" - width       %d", seq_start->width);
        dvb_log(" - height      %d", seq_start->height);
        dvb_log(" - aspect      %d", seq_start->aspect);
        dvb_log(" - framerate   %d", seq_start->framerate);
        dvb_log(" - bitrate     %d", seq_start->bitrate);
        dvb_log(" - one         %d", seq_start->one);
        dvb_log(" - vbv         %d", seq_start->vbv);
        dvb_log(" - constrained %d", seq_start->constrained);
        dvb_log(" - qm_intra    %d", seq_start->qm_intra);
        dvb_log(" - qm_nonintra %d", seq_start->qm_nonintra);
}

const char *dvb_mpeg_es_frame_names[5] = {
	"?",
	"I",
	"P",
	"B",
	"D"
};

int dvb_mpeg_es_pic_start_init(const uint8_t *buf, ssize_t buflen, struct dvb_mpeg_es_pic_start *pic_start)
{
	if(buflen < sizeof(struct dvb_mpeg_es_pic_start))
		return -1;
	memcpy(pic_start, buf, sizeof(struct dvb_mpeg_es_pic_start));
	bswap32(pic_start->bitfield);
	bswap32(pic_start->bitfield2);
	return 0;
}

void dvb_mpeg_es_pic_start_print(struct dvb_v5_fe_parms *parms, struct dvb_mpeg_es_pic_start *pic_start)
{
	dvb_log("MPEG ES PIC START");
        dvb_log(" - temporal_ref %d", pic_start->temporal_ref);
        dvb_log(" - coding_type  %d (%s-frame)", pic_start->coding_type, dvb_mpeg_es_frame_names[pic_start->coding_type]);
        dvb_log(" - vbv_delay    %d", pic_start->vbv_delay);
}