/*
 * Copyright (c) 2011-2012 - Mauro Carvalho Chehab <mchehab@redhat.com>
 * Copyright (c) 2012 - Andre Roth <neolynx@gmail.com>
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

#include "descriptors/sdt.h"
#include "dvb-fe.h"

void dvb_table_sdt_init(struct dvb_v5_fe_parms *parms, const uint8_t *ptr, ssize_t size, uint8_t **buf, ssize_t *buflen)
{
	uint8_t *d;
	const uint8_t *p = ptr;
	struct dvb_table_sdt *sdt = (struct dvb_table_sdt *) ptr;
	struct dvb_table_sdt_service **head;

	bswap16(sdt->network_id);

	if (!*buf) {
		d = malloc(DVB_MAX_PAYLOAD_PACKET_SIZE * 2);
		*buf = d;
		*buflen = 0;
		sdt = (struct dvb_table_sdt *) d;
		memcpy(sdt, p, sizeof(struct dvb_table_sdt) - sizeof(sdt->service));
		*buflen += sizeof(struct dvb_table_sdt);

		sdt->service = NULL;
		head = &sdt->service;
	} else {
		// should realloc d
		d = *buf;

		/* find end of curent list */
		sdt = (struct dvb_table_sdt *) d;
		head = &sdt->service;
		while (*head != NULL)
			head = &(*head)->next;

		/* read new table */
		sdt = (struct dvb_table_sdt *) p;
	}
	p += sizeof(struct dvb_table_sdt) - sizeof(sdt->service);

	struct dvb_table_sdt_service *last = NULL;
	while ((uint8_t *) p < ptr + size - 4) {
		struct dvb_table_sdt_service *service = (struct dvb_table_sdt_service *) (d + *buflen);
		memcpy(d + *buflen, p, sizeof(struct dvb_table_sdt_service) - sizeof(service->descriptor) - sizeof(service->next));
		p += sizeof(struct dvb_table_sdt_service) - sizeof(service->descriptor) - sizeof(service->next);
		*buflen += sizeof(struct dvb_table_sdt_service);

		bswap16(service->service_id);
		bswap16(service->bitfield);
		service->descriptor = NULL;
		service->next = NULL;

		if(!*head)
			*head = service;
		if(last)
			last->next = service;

		/* get the descriptors for each program */
		struct dvb_desc **head_desc = &service->descriptor;
		*buflen += dvb_parse_descriptors(parms, p, d + *buflen, service->section_length, head_desc);

		p += service->section_length;
		last = service;
	}
}

void dvb_table_sdt_print(struct dvb_v5_fe_parms *parms, struct dvb_table_sdt *sdt)
{
	dvb_log("SDT");
	dvb_table_header_print(parms, &sdt->header);
	dvb_log("|- network_id         %d", sdt->network_id);
	dvb_log("|\\  service_id");
	const struct dvb_table_sdt_service *service = sdt->service;
	uint16_t services = 0;
	while(service) {
		dvb_log("|- %7d", service->service_id);
		dvb_log("|   EIT schedule          %d", service->EIT_schedule);
		dvb_log("|   EIT present following %d", service->EIT_present_following);
		dvb_log("|   free CA mode          %d", service->free_CA_mode);
		dvb_log("|   running status        %d", service->running_status);
		dvb_print_descriptors(parms, service->descriptor);
		service = service->next;
		services++;
	}
	dvb_log("|_  %d services", services);
}
